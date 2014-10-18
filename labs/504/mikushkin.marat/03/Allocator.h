#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <iostream>
#include "MemoryBlock.h"

class Allocator {
public:
	Allocator(size_t maximalSize);
	int allocate(MemoryBlock* root, size_t bytes);
	void merge(MemoryBlock* block);
	int free(MemoryBlock* cur, size_t data_ptr);
	void clearMemory(MemoryBlock* block);
	void info(MemoryBlock& block, size_t& blocks_count,
			size_t& total_memory_alloc, size_t& max_block_size);
	void map(MemoryBlock& block);
private:
	size_t size;
};

#endif
