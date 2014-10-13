#ifndef _ALLOC_H
#define _ALLOC_H

typedef struct
{
    void    *buf;
    size_t  size;
    size_t  first_block;
} alloc;

typedef struct
{
    size_t  prev_offset;
    size_t  end_offset;
    size_t  next_offset;
} block;

alloc* create_allocator(size_t size);

int allocate(alloc *allocator, size_t size, size_t *offset);

int free_space(alloc *allocator, size_t offset);

void info(alloc *allocator);

void map(alloc *allocator);

#endif