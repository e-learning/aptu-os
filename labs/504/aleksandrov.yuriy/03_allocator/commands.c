#include "commands.h"
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <stddef.h>
#include <stdbool.h> // bool type

struct MCB {
	size_t size;
	MCB *next;
	MCB *prev;
	bool isAvailable;
};

void * init_buffer(size_t buffer_size)
{
	void * buffer;
	if (buffer_size <= sizeof(MCB))
	{ // too small size
		buffer = NULL;
		return buffer;
	}
	
	buffer = malloc(buffer_size);
	MCB* first_block = ((MCB*)buffer);
		
	first_block->size = buffer_size - sizeof(MCB);
	first_block->next = NULL;
	first_block->prev = NULL;
	first_block->isAvailable = true;
	
	return buffer;
}


void show_help()
{
	printf("Commands:\n");
	printf("   alloc S  allocate S bytes if possible and return pointer to head.\n");
	printf("   free P   free memory by pointer P.\n");
	printf("   info     show statistics of buffer usage (number of allocated blocks, \n            total memory size of allocated blocks and maximal available memory size).\n");
	printf("   map      show buffer map (u = allocated byte, m = service byte, f = free byte)\n");
}

size_t my_alloc(void * buffer, size_t alloc_size)
{
	MCB *mcb = (MCB*)buffer;
	while ( mcb != NULL )
	{ // first-fit approach
		if  (mcb->isAvailable && mcb->size >= alloc_size)
		{
			if (mcb->size >= alloc_size && mcb->size <= alloc_size +sizeof(MCB))
			{ // only one block possible, so mcb->size is greater or equal to alloc_size
				mcb->isAvailable = false;
			} else {
				MCB * newMCB = (MCB*)((size_t)(mcb+1)+alloc_size);
				newMCB->next = mcb->next;
				newMCB->prev = mcb;
				newMCB->size = mcb->size - alloc_size - sizeof(MCB);
				newMCB->isAvailable = true;
				
				mcb->size = alloc_size;
				mcb->isAvailable = false;
				mcb->next = newMCB;
			}
			return (size_t)(mcb+1)-(size_t) buffer;
		}
		mcb = mcb -> next;
	} 
	return 0;	
}

int my_free(void * buffer, size_t data_pointer)
{
	size_t pointer = (((size_t)data_pointer)+((size_t) buffer));
	
	// check if data_pointer is really start of some allocated block
	MCB *mcb = (MCB*) buffer;
	while ( mcb != NULL && ((size_t)(mcb+1)) != pointer)
		mcb = mcb->next;
	
	if (mcb != NULL && mcb->isAvailable == false )
	{
		mcb->isAvailable = true;
		
		// merge current, previous and next available blocks
		MCB* nextMCB = mcb->next;
		size_t addSize = 0;
		while( nextMCB != NULL && nextMCB -> isAvailable == true )
		{
			addSize += nextMCB->size + sizeof(MCB);
			nextMCB = nextMCB -> next;
		}
		if (addSize > 0)
		{
			mcb -> size += addSize;
			mcb -> next = nextMCB;
			if (nextMCB != NULL)
				nextMCB -> prev = mcb;
		}
		MCB* prevMCB = mcb->prev;
		addSize = 0;
		while( prevMCB != NULL && prevMCB -> isAvailable == true )
		{
			addSize += prevMCB->size + sizeof(MCB);
			if (prevMCB -> prev == NULL)
				break;
			else
				prevMCB = prevMCB -> prev;
		}
		if (addSize > 0)
		{
			if (prevMCB != NULL)
			{
				prevMCB -> size = addSize + mcb->size;
				prevMCB->isAvailable = true;
				prevMCB -> next = mcb->next;
			}
			if (mcb->next != NULL)
				(mcb->next) -> prev = prevMCB;
		}
		return 0;
	} else
		return -1;
}

void show_info(void* buffer, size_t buffer_size)
{
	size_t count_allocated_mcb = 0;
	size_t max_available_memory = 0;
	size_t allocated_memory = 0;
	MCB* mcb = (MCB*) buffer;
	while (1)
	{
		if (mcb-> isAvailable == false)
		{ // allocated
			++count_allocated_mcb;
			allocated_memory += mcb->size;
		}
		else
		{ 
			if ( mcb-> size > max_available_memory)
				max_available_memory = mcb-> size;
		}
		
		if (mcb->next != NULL)
			mcb = mcb->next;
		else
			break;
	}
	printf("%zu %zu %zu\n", count_allocated_mcb, allocated_memory, max_available_memory);
}
void show_map(void* buffer, size_t buffer_size)
{
	MCB* mcb = (MCB*)buffer;
	while (mcb != NULL)
	{
		for(int i=sizeof(MCB);i--;printf("m"));
		if (mcb->isAvailable)
			for(int i=mcb->size;i--;printf("f"));
		else
			for(int i=mcb->size;i--;printf("u"));
		mcb = mcb->next;
	}
	printf("\n");
	
}
