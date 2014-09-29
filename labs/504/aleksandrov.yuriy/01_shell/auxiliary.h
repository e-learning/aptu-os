#pragma once

#include <string.h> // size_t

int execute(char** arglist);

/* 
 * convert command line's string to array of strings 
 * (command's names, keys, arguments) 
 * using only spaces and quotes
 * */
char** split_str(char* str, size_t* arg_number); 
