#include <stdio.h>

#include "command.h"

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    printf("Usage: ./redir <input> <command> <output>");
    return 1;
  }

  FILE * input_file = fopen(argv[1], "r");
  char * command = argv[2];
  FILE * output_file = fopen(argv[3], "w");

  char * result = execute_command(command, input_file);
  
  fclose(input_file);
  fprintf(output_file, "%s", result);
  fclose(output_file);
  return 0;
}
