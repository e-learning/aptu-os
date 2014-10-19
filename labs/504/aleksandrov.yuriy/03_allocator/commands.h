#ifndef __ALLOCATOR_COMMANDS__H__
#define __ALLOCATOR_COMMANDS__H__

#include <stddef.h> // size_t
#include <stdbool.h> // bool type

typedef struct MCB MCB;

void * init_buffer(size_t buffer_size);

void show_help();

size_t my_alloc(void * buffer, size_t alloc_size);

int my_free(void * buffer, size_t data_pointer);

void show_info();

void show_map(void* buffer, size_t buffer_size);


#endif
