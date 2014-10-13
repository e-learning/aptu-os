#ifndef __ALLOCATOR_AUXILIARY__H__
#define __ALLOCATOR_AUXILIARY__H__

#include <string.h> // size_t

char** split_str(char* str, size_t* arg_number); 

void free_commandlist(char** commandlist, size_t commandsnum);


#endif
