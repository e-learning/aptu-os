#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

enum Commands
{
    ALLOC,
    FREE,
    MAP,
    INFO,
    EXIT,
};


struct MCB
{
    size_t  bytes_before_next; //ignore size of MCB
    MCB*    next_mcb;
    bool    is_free;
};

struct Allocator
{
    char*       memory_block;
    size_t      free_size;
    size_t      size;
    size_t      user_blocks;
    size_t      memory_allocated;
};

#endif // COMMON_H
