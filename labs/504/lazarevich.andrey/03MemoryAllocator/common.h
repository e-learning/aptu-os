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
    u_int16_t  bytes_before_next; //ignore size of MCB
    MCB*      next_mcb;
    bool      is_free;
};

struct Allocator
{
    char*       memory_block;
    u_int16_t    free_size;
    u_int16_t    size;
    u_int16_t    user_blocks;
    u_int16_t    memory_allocated;
};

#endif // COMMON_H
