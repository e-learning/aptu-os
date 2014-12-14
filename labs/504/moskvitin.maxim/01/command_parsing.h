#ifndef __COMMAND_PARSING_H_
#define __COMMAND_PARSING_H_

#include "Vector.h"
#include <stdbool.h>

struct command
{
    char* command_name;
    struct Vector* args;

    bool input_pipe;
    bool output_pipe;
    bool input_file;
    bool output_file;
    char* input_file_name;
    char* output_file_name;
};

struct command* create_command();
void free_command(struct command*);

struct Vector* parse_command(char* command);

#endif
