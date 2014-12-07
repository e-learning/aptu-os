#ifndef SH_H
#define SH_H

#include <dirent.h>
#include <stdio.h>
#include "commands.h"

#define MAX_COMMAND_LENGTH 512
#define TOKEN_SIZE 512
#define TOKEN_COUNT 10

char * tokens[TOKEN_COUNT];

int tokenize(char * string);
void free_tokens();
void signal_handler(int sig);
void run_command(int argc, char ** argv);
void run();

#endif