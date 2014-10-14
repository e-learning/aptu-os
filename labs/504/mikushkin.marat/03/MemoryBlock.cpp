#include "MemoryBlock.h"

MemoryBlock::MemoryBlock(size_t blockSize) {
	pointer = 0;
	next = 0;
	isFree = true;
	this->size = blockSize;
}

size_t MemoryBlock::getSize() {
	return (next ? next->pointer - getPointer() : size - getPointer());
}

size_t MemoryBlock::getPointer() {
	return pointer + sizeof(MemoryBlock);
}
