#pragma once
#include <stdint.h>
struct entry_table
{
    unsigned size;
    unsigned* content;
};

struct descriptor_table
{
    unsigned size;
    uint64_t *content;
};