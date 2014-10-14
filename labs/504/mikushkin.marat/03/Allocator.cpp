#include "Allocator.h"
#include "MemoryBlock.h"

using namespace std;

Allocator::Allocator(size_t maximalSize) {
	size = maximalSize;
}

int Allocator::allocate(MemoryBlock * block, size_t bytes) {
	if (!block) {
		return -1;
	}

	if (block->isFree && bytes <= block->getSize()) {
		block->isFree = false;
		if (block->getPointer() + bytes + sizeof(MemoryBlock)
				< (block->next ? block->next->pointer : size)) {
			MemoryBlock* data_block = new MemoryBlock(size);
			data_block->pointer = block->pointer + sizeof(MemoryBlock) + bytes;
			data_block->next = block->next;
			block->next = data_block;
		}
		return block->getPointer();
	} else {
		return allocate(block->next, bytes);
	}
}

void Allocator::merge(MemoryBlock * block) {
	MemoryBlock * next = block->next;
	block->next = next->next;
	delete next;
}

int Allocator::free(MemoryBlock * current, size_t pointer) {
	if (!current) {
		return -1;
	}

	if (current->getPointer() == pointer) {
		current->isFree = true;
		if (current->next && current->next->isFree) {
			merge(current);
		}
		return 0;
	} else {
		int result = free(current->next, pointer);
		if (!result && current->isFree && current->next && current->next->isFree) {
			merge(current);
		}
		return result;
	}
}

void Allocator::clearMemory(MemoryBlock * block) {
	if (!block->next) {
		return;

	}
	clearMemory(block->next);
	delete block;
}

void Allocator::info(MemoryBlock & block, size_t & numberOfBlocks,
		size_t & memoryAllocated, size_t & maximalSizeOfBlock) {

	MemoryBlock * current = &block;
	for (;;) {
		if (!current->isFree) {
			++numberOfBlocks;
			memoryAllocated += current->getSize();
		} else if (maximalSizeOfBlock < current->getSize()) {
			maximalSizeOfBlock = current->getSize();
		}
		if (current->next) {
			current = current->next;
		} else {
			break;
		}
	}
}

void Allocator::map(MemoryBlock & block) {
	MemoryBlock * current = &block;
	for (;;) {
		for (size_t i = 0; i < sizeof(MemoryBlock); ++i) {
			cout << 'm';
		}
		for (size_t i = 0; i < current->getSize(); ++i) {
			cout << (current->isFree ? 'f' : 'u');
		}
		if (current->next) {
			current = current->next;
		} else {
			break;
		}
	}
}
