#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include "alloc.h"

alloc* create_allocator(size_t size)
{
    alloc* allocator = malloc(sizeof(alloc));
    allocator->buf   = malloc(size);
    allocator->size   = size;
    allocator->first_block = size;
    return allocator;
}


int allocate(alloc *allocator, size_t size, size_t *offset)
{
    block *new_block = NULL;
    if (allocator->first_block == allocator->size)
    {
        if (allocator->size >= size + sizeof(block))
        {
            new_block = (block*)allocator->buf;
            new_block->end_offset = sizeof(block) + size;
            new_block->prev_offset = allocator->size;
            new_block->next_offset = allocator->size;
            allocator->first_block = 0;
            *offset = sizeof(block);
            return 0;
        }
        *offset = allocator->size;
        return -1;
    }
    if (allocator->first_block >= size + sizeof(block))
    {
        block* next_block = (block*) (allocator->buf + allocator->first_block);
        new_block = (block*)allocator->buf;
        new_block->end_offset  = sizeof(block) + size;
        new_block->prev_offset = allocator->size;
        new_block->next_offset = allocator->first_block;
        allocator->first_block = 0;
        next_block->prev_offset = 0;
        *offset = sizeof(block);
        return 0;
    }
    size_t current_offset = allocator->first_block;
    block* current_block  = (block*)(allocator->buf + current_offset);
    while (current_block->next_offset - current_block->end_offset < size + sizeof(block))
    {
        if (current_block->next_offset == allocator->size)
        {
            *offset = allocator->size;
            return -1;
        }
        current_offset = current_block->next_offset;
        current_block  = (block*)(allocator->buf + current_offset);
    }

    *offset = current_block->end_offset;
    new_block  = (block*)(allocator->buf + *offset);
    new_block->end_offset = sizeof(block) + (*offset) + size;
    new_block->next_offset = current_block->next_offset;
    new_block->prev_offset = current_offset;

    if (current_block->next_offset < allocator->size)
    {
        block* next_block = (block*) (allocator->buf + current_block->next_offset);
        next_block->prev_offset = *offset;
    }

    current_block->next_offset = *offset;

    *offset += sizeof(block);
    return 0;
}

int free_space(alloc *allocator, size_t offset)
{
    if (allocator->first_block == allocator->size)
    {
        return -1;
    }

    size_t current_offset = allocator->first_block;
    while (current_offset + sizeof(block) < offset)
    {
        block* current_block = (block*) (allocator->buf + current_offset);
        if (current_block->next_offset == allocator->size)
        {
            return -1;
        }
        current_offset = current_block->next_offset;
    }

    if (current_offset + sizeof(block) == offset)
    {
        block* current_block = (block*) (allocator->buf + current_offset);
        if (current_block->prev_offset == allocator->size)
        {
            allocator->first_block = current_block->next_offset;
        }
        else
        {
            block* prev_block = (block*) (allocator->buf + current_block->prev_offset);
            prev_block->next_offset = current_block->next_offset;
        }

        if (current_block->next_offset != allocator->size)
        {
            block* next_block = (block*) (allocator->buf + current_block->next_offset);
            next_block->prev_offset = current_block->prev_offset;
        }
        return 0;
    }
    return -1;
}

int max(int a, int b)
{
    if (a < b)
        return b;
    return a;
}

void info(alloc *allocator)
{
    int block_count = 0;
    int occuped_memory = 0;
    int max_alloc = 0;
    if (allocator->first_block == allocator->size)
    {
        max_alloc = allocator->size - sizeof(block);
    }
    else
    {
        size_t offset = allocator->first_block;
        max_alloc = max(max_alloc, (int)offset - sizeof(block));
        while (offset != allocator->size)
        {
            block *current = (block*) (allocator->buf + offset);
            ++block_count;
            occuped_memory += current->end_offset - offset - sizeof(block);
            max_alloc = max(max_alloc, (int)current->next_offset - current->end_offset - sizeof(block));
            offset = current->next_offset;
        }
    }
    printf("%d %d %d\n", block_count, occuped_memory, max_alloc);
}

void map(alloc *allocator)
{
    size_t offset = 0;
    size_t next_block = allocator->first_block;
    for (; offset < allocator->size;)
    {
        for (; offset < allocator->size && offset < next_block; ++offset)
        {
            putc('f', stdout);
        }
        if (offset != allocator->size)
        {
            block* current = (block*) (allocator->buf + offset);
            next_block = current->next_offset;
            size_t i = 0;
            for (; i < sizeof(block); ++i, ++offset)
            {
                putc('m', stdout);
            }
            for (; offset < current->end_offset; ++offset)
            {
                putc('u', stdout);
            }
        }
    }
    putc('\n', stdout);
}