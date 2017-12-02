#ifndef COMMANDS_H
#define COMMANDS_H

#include <SPI.h>
#include <SD.h>

enum status
{
    STATUS_FAIL = 0,
    STATUS_OK   = 1
};

typedef status (*fct_ptr)(char**);

int freeRam();
bool    set_current_directory(String);
uint8_t get_len_commands_array();
status  mkdir(char**);
status  ls(char**); 
status  cd(char**);

const char* const available_commands_str[] = {
    "mkdir",
    "ls",
    "cd"
};

const fct_ptr commands_ptr[] = {
    &mkdir,
    &ls,
    &cd
};

#endif

