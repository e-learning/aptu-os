#ifndef ALLOCATOR_H
#define ALLOCATOR_H

struct allocator
{
    char *buffer;
    unsigned int size;
    char *first_empty;
    char *first_full;
};

struct stat
{
    unsigned int used_blocks;
    unsigned int used_mem;
    unsigned int max_alloc;
};

int init_allocator(struct allocator *allocator, unsigned int buffer_size);

void destroy_allocator(struct allocator *allocator);

char* alloc(struct allocator *allocator, unsigned int size);

int allocator_free(struct allocator *allocator, char *p);

struct stat info(struct allocator *allocator);

char* map(struct allocator *allocator);

#endif
