#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#include "allocator.h"


struct memory_block
{
	struct memory_block* next;
	struct memory_block* prev;
	unsigned size;
	void* data_ptr;
	char is_service;
};

#define BLK_SIZE (sizeof(struct memory_block))
#define FREE_BYTE ('f')
#define USER_BYTE ('u')
#define SERVICE_BYTE ('m')

void* buffer;
unsigned buffer_size;
struct memory_block* first_block;
struct memory_block* last_block;


void init_allocator(unsigned new_buffer_size)
{
	buffer_size = new_buffer_size;
	buffer = malloc(buffer_size);
	first_block = (struct memory_block*)buffer;
	last_block = (struct memory_block*)(buffer + buffer_size - BLK_SIZE);
	
	first_block->next = last_block;
	first_block->prev = first_block;
	first_block->size = BLK_SIZE;
	first_block->data_ptr = (void*)first_block + BLK_SIZE;
	first_block->is_service = 1;

	last_block->next = last_block;
	last_block->prev = first_block;
	last_block->size = BLK_SIZE;
	last_block->data_ptr = (void*)last_block + BLK_SIZE;
	last_block->is_service = 1;
}


void deinit_allocator()
{
	free(buffer);
}


struct memory_block* append_block_after(struct memory_block* blk, unsigned size)
{
	struct memory_block* new_block;
	new_block = (struct memory_block*)((void*)blk + blk->size);

	new_block->prev = blk;
	new_block->next = blk->next;
	new_block->size = BLK_SIZE + size;
	new_block->data_ptr = (void*)new_block + BLK_SIZE;
	new_block->is_service = 0;

	blk->next->prev = new_block;
	blk->next = new_block;

	return new_block;
}


unsigned get_free_size_after(struct memory_block* blk)
{
	return (void*)(blk->next) - ((void*)blk + blk->size);
}


void* alloc_mem(unsigned size)
{
	struct memory_block* blk;
	struct memory_block* new_block;
	for (blk = first_block; blk != last_block; blk = blk->next)
	{
		if (get_free_size_after(blk) >= size + BLK_SIZE)
		{
			new_block = append_block_after(blk, size);
			return new_block->data_ptr;
		}
	}
	return NULL;
}


void delete_block(struct memory_block* blk)
{
	struct memory_block* prev;
	struct memory_block* next;
	next = blk->next;
	prev = blk->prev;

	prev->next = next;
	next->prev = prev;
}


int free_mem(void* ptr)
{
	struct memory_block* blk;
	for (blk = first_block; blk != last_block; blk = blk->next)
	{
		if (blk->data_ptr == ptr && !(blk->is_service))
		{
			delete_block(blk);
			return 0;
		}
	}
	return -1;
}


unsigned to_local_offset(void* ptr)
{
	return ptr - buffer;
}


void* from_local_offset(unsigned offset)
{
	return buffer + offset;
}


struct stat info()
{
	unsigned free_size;
	struct memory_block* blk;
	struct stat stat;
	stat.blk_count = 0;
	stat.allocated_memory = 0;
	stat.max_successful_alloc = 0;

	for (blk = first_block; blk != last_block; blk = blk->next)
	{
		if (!blk->is_service)
		{
			stat.blk_count++;
			stat.allocated_memory += blk->size - BLK_SIZE;
		}
		free_size = get_free_size_after(blk);
		if (free_size > stat.max_successful_alloc + BLK_SIZE)
		{
			stat.max_successful_alloc = free_size - BLK_SIZE;
		}
	}
	return stat;
}


void write_block_to_map(struct memory_block* blk, char* map)
{
	unsigned head_pos = to_local_offset((void*)blk);
	memset(map + head_pos, SERVICE_BYTE, BLK_SIZE);
	if (blk->is_service)
	{
		memset(map + head_pos + BLK_SIZE, SERVICE_BYTE, blk->size - BLK_SIZE);
	}
	else
	{
		memset(map + head_pos + BLK_SIZE, USER_BYTE, blk->size - BLK_SIZE);
	}
}


char* map()
{
	struct memory_block* blk;
	char* memory_map = malloc(buffer_size + 1);
	memset(memory_map, FREE_BYTE, buffer_size);
	memory_map[buffer_size] = '\0';
	for (blk = first_block; blk != last_block; blk = blk->next)
	{
		write_block_to_map(blk, memory_map);
	}
	write_block_to_map(last_block, memory_map);
	return memory_map;
}