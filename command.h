#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

#define BIN_PATH_OVERHEAD 6
#define MAX_ARGS 100
#define MAX_BUFFER 1024

char *execute_command(char *command, FILE *input_file);

#endif // COMMAND_H
