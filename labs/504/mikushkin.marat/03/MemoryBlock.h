#ifndef MEMORYBLOCK_H_
#define MEMORYBLOCK_H_

#include <string>

class MemoryBlock {
public:
	size_t pointer;
	MemoryBlock * next;
	bool isFree;
	size_t size;

	MemoryBlock(size_t blockSize);
	size_t getSize();
	size_t getPointer();
};

#endif
