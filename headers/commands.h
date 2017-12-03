#ifndef COMMANDS_H
#define COMMANDS_H

#include "bmp_printer.h"

enum status
{
    STATUS_FAIL = 0,
    STATUS_OK   = 1
};

typedef status (*fct_ptr)(char**);

int     freeRam();
bool    set_current_directory(const char*);
uint8_t get_len_commands_array();

status  mkdir(char**);
status  rmdir(char**);
status  ls   (char**); 
status  cd   (char**);
status  cat  (char**);
status  echo (char**);
status  find (char**);
status  cp   (char**);
status  crc  (char**);

const char* const available_commands_str[] = {
    "mkdir",
    "rmdir",
    "ls",
    "cd",
    "cat",
    "echo",
    "find",
    "cp",
    "crc"
};

const fct_ptr commands_ptr[] = {
    &mkdir,
    &rmdir,
    &ls,
    &cd,
    &cat,
    &echo,
    &find,
    &cp,
    &crc
};

#endif

