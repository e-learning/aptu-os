#include "allocator.h"
#include <malloc.h>
#include <memory.h>

struct empty_block_header
{
    struct empty_block_header *next_empty;
    struct empty_block_header *prev_empty;
    unsigned int size;
};

struct full_block_header
{
    struct full_block_header *next_full;
    struct full_block_header *prev_full;
    struct empty_block_header *next_empty;
    struct empty_block_header *prev_empty;
    unsigned int size;
};

int init_allocator(struct allocator *allocator, unsigned int buffer_size)
{
    if(buffer_size < sizeof(struct empty_block_header))
    {
        return -1;
    }
    allocator->buffer = (char*) malloc(sizeof(char) * buffer_size);
    if(!allocator->buffer)
    {
        return -2;
    }
    allocator->size = buffer_size;
    allocator->first_full = 0;
    allocator->first_empty = allocator->buffer;
    struct empty_block_header em_header;
    em_header.next_empty = 0;
    em_header.prev_empty = 0;
    em_header.size = allocator->size - sizeof(struct empty_block_header);
    memcpy(allocator->buffer, &em_header, sizeof(struct empty_block_header));
    return 0;
}

void destroy_allocator(struct allocator *allocator)
{
    free(allocator->buffer);
}

char* alloc(struct allocator *allocator, unsigned int size)
{
    struct empty_block_header *current, *new_empty_header;
    for(current = (struct empty_block_header*)allocator->first_empty; current; current = current->next_empty)
    {
        if(current->size + sizeof(struct empty_block_header) >= size + sizeof(struct full_block_header))
        {
            break;
        }
    }
    if(!current)
    {
        return 0;
    }
    struct empty_block_header removed_header = *current;
    struct full_block_header *full_header = (struct full_block_header*)current;
    if(removed_header.size >= size + sizeof(struct full_block_header))
    {
        new_empty_header = (struct empty_block_header*)((char*)current + size + sizeof(struct full_block_header));
        new_empty_header->next_empty = removed_header.next_empty;
        new_empty_header->prev_empty = removed_header.prev_empty;
        new_empty_header->size = removed_header.size - size - sizeof(struct full_block_header);
        if(new_empty_header->prev_empty)
        {
            new_empty_header->prev_empty->next_empty = new_empty_header;
        }
        else
        {
            allocator->first_empty = (char*)new_empty_header;
        }
        if(new_empty_header->next_empty)
        {
            new_empty_header->next_empty->prev_empty = new_empty_header;
        }
        full_header->size = size;
        full_header->next_empty = new_empty_header;
        full_header->prev_empty = removed_header.prev_empty;
    }
    else
    {
        if(removed_header.next_empty)
        {
            removed_header.next_empty->prev_empty = removed_header.prev_empty;
        }
        if(removed_header.prev_empty)
        {
            removed_header.prev_empty->next_empty = removed_header.next_empty;
        }
        else
        {
            allocator->first_empty = (char*)removed_header.next_empty;
        }
        full_header->next_empty = removed_header.next_empty;
        full_header->prev_empty = removed_header.prev_empty;
        full_header->size = removed_header.size  - sizeof(struct full_block_header) + sizeof(struct empty_block_header);
    }

    full_header->prev_full = 0;
    full_header->next_full = (struct full_block_header*)allocator->first_full;
    if(allocator->first_full)
    {
        ((struct full_block_header *)allocator->first_full)->prev_full = full_header;
    }
    allocator->first_full = (char*)full_header;
    return (char*)full_header + sizeof(struct full_block_header);
}

int allocator_free(struct allocator *allocator, char *p)
{
    if(!p)
    {
        return 0;
    }
    struct full_block_header *current_full;
    for(current_full = (struct full_block_header*)allocator->first_full; current_full; current_full = current_full->next_full)
    {
        if(p == (char*)current_full + sizeof(struct full_block_header))
        {
            break;
        }
    }
    if(!current_full)
    {
        return -1;
    }
    if(current_full->prev_full)
    {
        current_full->prev_full->next_full = current_full->next_full;
    }
    else
    {
        allocator->first_full = (char*)current_full->next_full;
    }
    if(current_full->next_full)
    {
        current_full->next_full->prev_full = current_full->prev_full;
    }

    struct full_block_header removed_header = *current_full;
    struct empty_block_header *new_empty = (struct empty_block_header*)current_full;
    new_empty->next_empty = removed_header.next_empty;
    new_empty->prev_empty = removed_header.prev_empty;
    new_empty->size = removed_header.size + sizeof(struct full_block_header) - sizeof(struct empty_block_header);
    if(!new_empty->prev_empty)
    {
        allocator->first_empty = (char*)new_empty;
    }

    if((char*)new_empty + new_empty->size + sizeof(struct empty_block_header) == (char*)new_empty->next_empty)
    {
        new_empty->size += sizeof(struct empty_block_header) + new_empty->next_empty->size;
        new_empty->next_empty = new_empty->next_empty->next_empty;
    }
    if(new_empty->prev_empty && (char*)new_empty->prev_empty + new_empty->prev_empty->size + sizeof(struct empty_block_header) == (char*)new_empty)
    {
        new_empty->prev_empty->size += sizeof(struct empty_block_header) + new_empty->size;
        new_empty->prev_empty->next_empty = new_empty->next_empty;
    }
    return 0;
}

struct stat info(struct allocator *allocator)
{
    struct stat stat;
    memset(&stat, 0, sizeof(struct stat));
    struct empty_block_header *current;
    for(current = (struct empty_block_header*)allocator->first_empty; current; current = current->next_empty)
    {
        if(current->size + sizeof(struct empty_block_header) > stat.max_alloc + sizeof(struct full_block_header))
        {
            stat.max_alloc = current->size + sizeof(struct empty_block_header) - sizeof(struct full_block_header);
        }
    }

    struct full_block_header *current_full;
    for(current_full = (struct full_block_header*)allocator->first_full; current_full; current_full = current_full->next_full)
    {
        ++stat.used_blocks;
        stat.used_mem += current_full->size;
    }
    return stat;
}

char* map(struct allocator *allocator)
{
    char *map_string = malloc((allocator->size + 1) * sizeof(char));
    if(!map_string)
    {
        return 0;
    }
    map_string[allocator->size] = '\0';

    char *buffer_pointer;
    struct empty_block_header *current_empty;
    for(current_empty = (struct empty_block_header*)allocator->first_empty; current_empty; current_empty = current_empty->next_empty)
    {
        buffer_pointer = map_string + ((char*) current_empty - allocator->buffer);
        memset(buffer_pointer, 'm', sizeof(struct empty_block_header));
        memset(buffer_pointer + sizeof(struct empty_block_header), 'f', current_empty->size);
    }

    struct full_block_header *current_full;
    for(current_full = (struct full_block_header*)allocator->first_full; current_full; current_full = current_full->next_full)
    {
        buffer_pointer = map_string + ((char*) current_full - allocator->buffer);
        memset(buffer_pointer, 'm', sizeof(struct full_block_header));
        memset(buffer_pointer + sizeof(struct full_block_header), 'u', current_full->size);
    }
    return map_string;
}
