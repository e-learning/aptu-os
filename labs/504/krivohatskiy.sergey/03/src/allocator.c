#include "allocator.h"
#include <malloc.h>
#include <memory.h>

struct full_block_header
{
    struct full_block_header *next_full;
    struct full_block_header *prev_full;
    unsigned int size;
};

int init_allocator(struct allocator *allocator, unsigned int buffer_size)
{
}

void destroy_allocator(struct allocator *allocator)
{
    free(allocator->buffer);
}

int alloc(struct allocator *allocator, unsigned int size)
{
}

int allocator_free(struct allocator *allocator, int p)
{
}

struct stat info(struct allocator *allocator)
{
}

char* map(struct allocator *allocator)
{
}

void defrag(struct allocator *allocator)
{
}
