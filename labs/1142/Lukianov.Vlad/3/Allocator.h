#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>

struct MemoryBlockDiscriptor
{
    bool free;
    size_t size;
    size_t used;
    MemoryBlockDiscriptor *next;
    size_t data_pointer;
};

class Allocator {
public:
    Allocator(size_t n);
    ~Allocator();
    int alloc(int size);
    void info();
    void map();
    int free(size_t ptr);
    void join_free_blocks();
private:
    size_t memory_allocated;
    char *memory;
    MemoryBlockDiscriptor *blks_list;
};

#endif