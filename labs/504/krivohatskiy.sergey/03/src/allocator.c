#include "allocator.h"
#include <malloc.h>
#include <memory.h>

struct full_block_header
{
    struct full_block_header *next_full;
    struct full_block_header *prev_full;
    unsigned int size;
    int id;
};

int init_allocator(struct allocator *allocator, unsigned int buffer_size)
{
    allocator->buffer = malloc(buffer_size);
    if(!allocator->buffer)
    {
        return -1;
    }
    allocator->first_full = 0;
    allocator->last_full = 0;
    allocator->size = buffer_size;
    allocator->full_blocks = 0;
    allocator->allocated_blocks = 0;
    return 0;
}

void destroy_allocator(struct allocator *allocator)
{
    free(allocator->buffer);
}

int alloc(struct allocator *allocator, unsigned int size)
{
    if(size + sizeof(struct full_block_header) > allocator->size)
    {
        return 0;
    }
    if(!allocator->full_blocks)
    {
        struct full_block_header *header = (struct full_block_header*)allocator->buffer;
        allocator->first_full = header;
        allocator->last_full = header;
        header->next_full = 0;
        header->prev_full = 0;
        header->size = size;
        allocator->full_blocks += 1;
        allocator->allocated_blocks += 1;
        header->id = allocator->allocated_blocks;
        return header->id;
    }

    unsigned int available_size = allocator->size - ((char*)allocator->last_full + allocator->last_full->size + sizeof(struct full_block_header) - allocator->buffer);
    if(available_size < size + sizeof(struct full_block_header))
    {
        return 0;
    }
    struct full_block_header *header = (struct full_block_header*)((char*)allocator->last_full + allocator->last_full->size + sizeof(struct full_block_header));

    header->next_full = 0;
    header->prev_full = allocator->last_full;
    allocator->last_full->next_full = header;
    header->size = size;

    allocator->last_full = header;
    allocator->full_blocks += 1;
    allocator->allocated_blocks += 1;
    header->id = allocator->allocated_blocks;
    return header->id;
}

char* allocator_get_pointer(struct allocator *allocator, int p)
{
    if(!p)
    {
        return 0;
    }
    if(p < 1 || p > allocator->allocated_blocks)
    {
        return 0;
    }
    struct full_block_header *header = allocator->first_full;
    while(header)
    {
        if(header->id == p)
        {
            break;
        }
        header = header->next_full;
    }
    if(!header)
    {
        return 0;
    }
    return (char*)header + sizeof(struct full_block_header);
}

int allocator_free(struct allocator *allocator, int p)
{
    if(p < 1 || p > allocator->allocated_blocks)
    {
        return -1;
    }
    struct full_block_header *header = allocator->first_full;
    while(header)
    {
        if(header->id == p)
        {
            break;
        }
        header = header->next_full;
    }
    if(!header)
    {
        return -1;
    }
    if(header->prev_full)
    {
        header->prev_full->next_full = header->next_full;
    }
    else
    {
        allocator->first_full = header->next_full;
    }
    if(header->next_full)
    {
        header->next_full->prev_full = header->prev_full;
    }
    else
    {
        allocator->last_full = header->prev_full;
    }
    allocator->full_blocks -= 1;
    defrag(allocator);
    return 0;
}

struct stat info(struct allocator *allocator)
{
    struct stat stat;
    if(allocator->last_full)
    {
        stat.max_alloc = allocator->size - ((char*)allocator->last_full + allocator->last_full->size + sizeof(struct full_block_header) - allocator->buffer);
    }
    else
    {
        stat.max_alloc = allocator->size;
    }
    if(stat.max_alloc <= sizeof(struct full_block_header))
    {
        stat.max_alloc = 0;
    }
    else
    {
        stat.max_alloc -= sizeof(struct full_block_header);
    }
    stat.used_blocks = allocator->full_blocks;
    stat.used_mem = 0;
    struct full_block_header *header = allocator->first_full;
    while(header)
    {
        stat.used_mem += header->size;
        header = header->next_full;
    }
    return stat;
}

char* map(struct allocator *allocator)
{
    char *map_str = malloc(allocator->size + 1);
    if(!map_str)
    {
        return map_str;
    }
    memset(map_str, 'f', allocator->size);
    map_str[allocator->size] = 0;
    char *cur_str_pos = map_str;
    struct full_block_header *header = allocator->first_full;
    while(header)
    {
        memset(cur_str_pos, 'm', sizeof(struct full_block_header));
        cur_str_pos += sizeof(struct full_block_header);
        memset(cur_str_pos, 'u', header->size);
        cur_str_pos += header->size;
        header = header->next_full;
    }
    return map_str;
}

void defrag(struct allocator *allocator)
{
    if(!allocator->first_full)
    {
        return;
    }
    if((char*)allocator->first_full != allocator->buffer)
    {
        memmove(allocator->buffer, (char*)allocator->first_full, allocator->first_full->size + sizeof(struct full_block_header));
        allocator->first_full = (struct full_block_header*)allocator->buffer;
        if(allocator->first_full->next_full)
        {
            allocator->first_full->next_full->prev_full = allocator->first_full;
        }
    }
    struct full_block_header *header = allocator->first_full;
    while(header->next_full)
    {
        if((char*)header + header->size + sizeof(struct full_block_header) == (char*)header->next_full)
        {
            header = header->next_full;
            continue;
        }
        memmove((char*)header + header->size + sizeof(struct full_block_header), (char*)header->next_full, header->next_full->size + sizeof(struct full_block_header));
        header->next_full = (struct full_block_header*)((char*)header + header->size + sizeof(struct full_block_header));

        if(header->next_full->next_full)
        {
            header->next_full->next_full->prev_full = header->next_full;
        }
        header = header->next_full;
    }
    allocator->last_full = header;
}
