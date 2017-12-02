#ifndef SHELL_H
#define SHELL_H

#include "Arduino.h"
#include "commands.h"

#define MAXIMUM_INPUT_BUFFER   200
#define MAXIMUM_ARGUMENT_LENGTH 50
#define MAXIMUM_ARGUMENTS_COUNT 10

enum state
{
    READ_CHAR,
    READ_QUOTE,
    READ_SPACE
};

void   handle_commands();
char*  read_line();
char** get_command_arguments(char*); 
status execute_command(char**);

#endif
