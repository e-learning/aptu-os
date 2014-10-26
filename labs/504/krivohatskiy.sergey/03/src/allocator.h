#ifndef ALLOCATOR_H
#define ALLOCATOR_H

struct full_block_header;
struct allocator
{
    char *buffer;
    unsigned int size;
    struct full_block_header *first_full;
};

struct stat
{
    unsigned int used_blocks;
    unsigned int used_mem;
    unsigned int max_alloc;
};

int init_allocator(struct allocator *allocator, unsigned int buffer_size);

void destroy_allocator(struct allocator *allocator);

void defrag(struct allocator *allocator);

int alloc(struct allocator *allocator, unsigned int size);

int allocator_free(struct allocator *allocator, int p);

struct stat info(struct allocator *allocator);

char* map(struct allocator *allocator);

#endif
