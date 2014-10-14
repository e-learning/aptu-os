#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "Allocator.h"
#include "MemoryBlock.h"

using namespace std;

class Processor {
public:
	Processor(Allocator * allocator, MemoryBlock * memoryBlock);
	void perform(string request);

	Allocator * allocatorPointer;
	MemoryBlock * memoryBlockPointer;
};

#endif
