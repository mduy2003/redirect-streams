#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "command.h"

char *execute_command(char *command, int input_fd)
{
  char *abosulute_command;
  size_t command_length = strlen(command);

  // Checks if the command is an absolute path. If not, prepend /bin/
  if (command[0] == '.')
  {
    char *current_directory = getcwd(NULL, 0);
    abosulute_command = malloc(strlen(current_directory) + command_length);
    strcpy(abosulute_command, current_directory);
    strcat(abosulute_command, command + 1);
    free(current_directory);
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

  // Split command into arguments
  char *token = strtok(abosulute_command, " ");
  char *args[MAX_ARGS];

  int ix = 0;
  while (token != NULL && ix < MAX_ARGS - 1)
  {
    args[ix++] = token;
    token = strtok(NULL, " ");
  }
  args[ix] = NULL; // Null-terminate

  int pipefd[2];
  pipe(pipefd);

  pid_t pid = fork();

  if (pid == 0) // Child process
  {
    if (input_fd != STDIN_FILENO)
    {
      dup2(input_fd, STDIN_FILENO); // Redirect stdin
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

    while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) // Reads from the pipe and adds to output
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
