#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "command.h"

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    printf("Usage: ./redir <input> <command> <output>\n");
    return 1;
  }

  int input_fd = open(argv[1], O_RDONLY);
  if (input_fd == -1)
  {
    perror("Failed to open input file");
    return 1;
  }

  char *command = argv[2];

  int output_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (output_fd == -1)
  {
    perror("Failed to open output file");
    close(input_fd);
    return 1;
  }

  char *result = execute_command(command, input_fd);

  close(input_fd);

  if (write(output_fd, result, strlen(result)) == -1) // Writes the result to the output file
  {
    perror("Failed to write to output file");
  }

  close(output_fd);
  free(result);

  return 0;
}
