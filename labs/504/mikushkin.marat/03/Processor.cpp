#include <iostream>
#include <string>
#include "Processor.h"
#include "Allocator.h"
#include "MemoryBlock.h"

using namespace std;

Processor::Processor(Allocator * allocator, MemoryBlock * memoryBlock) {
	allocatorPointer = allocator;
	memoryBlockPointer = memoryBlock;
}

void Processor::perform(string request) {
	if (request == "ALLOC") {
		size_t bytes = 0;
		cin >> bytes;

		int result = allocatorPointer->allocate(memoryBlockPointer, bytes);
		if (result < 0) {
			cout << "-" << endl;
		} else {
			cout << "+" << result << endl;
		}
	} else if (request == "FREE") {
		size_t pointer = 0;
		cin >> pointer;
		cout << (allocatorPointer->free(memoryBlockPointer, pointer) ? '-' : '+') << endl;

	} else if (request == "INFO") {
		size_t numberOfBlocks = 0;
		size_t memoryAllocated = 0;
		size_t maximalSizeOfBlock = 0;
		allocatorPointer->info(*memoryBlockPointer, numberOfBlocks, memoryAllocated,
				maximalSizeOfBlock);

		cout << numberOfBlocks << " " << memoryAllocated << " "
				<< maximalSizeOfBlock << endl;

	} else if (request == "MAP") {
		allocatorPointer->map(*memoryBlockPointer);
		cout << endl;

	} else if (request == "free memory") {
		allocatorPointer->clearMemory(memoryBlockPointer);
	} else {
		cout << "unknown request" << endl;
		return;
	}
}
