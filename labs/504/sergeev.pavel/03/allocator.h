#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

struct stat
{
	unsigned blk_count;
	unsigned allocated_memory;
	unsigned max_successful_alloc;
};


void init_allocator(unsigned new_buffer_size);
void* alloc_mem(unsigned size);
int free_mem(void* ptr);
unsigned to_local_offset(void* ptr);
void* from_local_offset(unsigned offset);
struct stat info();
char* map();

#endif