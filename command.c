#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "command.h"

char *execute_command(char *command, FILE *input_file)
{
  char *abosulute_command;
  size_t command_length = strlen(command);

  // Checks if the command is an absolute path. If not, prepend /bin/
  if (command[0] == '.')
  {
    char *currrent_directory = getcwd(NULL, 0);
    abosulute_command = malloc(strlen(currrent_directory) + command_length);
    strcpy(abosulute_command, currrent_directory);
    strcat(abosulute_command, command + 1);
    free(currrent_directory);
  }
  else if (command[0] == '/') 
  {
    abosulute_command = command;
  }
  else
  {
    abosulute_command = malloc(BIN_PATH_OVERHEAD + command_length);
    strcpy(abosulute_command, "/bin/");
    strcat(abosulute_command, command);
  }

  // Splits the command into separate words and stores them in an array
  char *token = strtok(abosulute_command, " ");
  char *args[MAX_ARGS];

  int ix = 0;
  while (token != NULL && ix < MAX_ARGS - 1)
  {
    args[ix++] = token;
    token = strtok(NULL, " ");
  }

  
  if (input_file != NULL) // Read from input file and add arguments to the array
  {
    char file_buffer[MAX_BUFFER];
    while (fgets(file_buffer, sizeof(file_buffer), input_file) != NULL && MAX_ARGS - 1) {
      file_buffer[strcspn(file_buffer, "\n")] = 0; // Remove newline character if present
      args[ix++] = file_buffer;
    }
  }
  args[ix] = NULL; // Null-terminate to indicate end

  int pipefd[2];
  pipe(pipefd);

  pid_t pid = fork();

  if (pid == 0) // Child process
  {

    if (input_file != NULL && input_file != stdin) 
    {
    int input_fd = fileno(input_file);
    dup2(input_fd, STDIN_FILENO); // Redirect stdin to input file
    close(input_fd);
    }

    close(pipefd[0]); 
    dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
    close(pipefd[1]);

    execve(args[0], args, NULL);
    perror("execve");
    exit(EXIT_FAILURE);
  }
  else // Parent process
  { 
    close(pipefd[1]);

    char buffer[MAX_BUFFER];
    size_t count;
    size_t total_size = 1; 
    char *output = malloc(total_size);
    output[0] = '\0'; 

    while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) // Reads from pipe and adds to output
    {
      buffer[count] = '\0'; 
      total_size += count;
      char *new_output = realloc(output, total_size);
      output = new_output;
      strcat(output, buffer);
    }
    close(pipefd[0]); 

    waitpid(pid, NULL, 0); 

    free(abosulute_command);
    return output;
  }
}
