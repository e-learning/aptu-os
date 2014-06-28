#ifndef MEM_STORAGE
#define MEM_STORAGE

#include <string>
#include "block.h"


struct MemInfo
{
public:
	int allocBlocks;
	int allocMemory;
	int maxAlloc;
	explicit MemInfo(int allocBlocks1 = 0, int allocMemory1 = 0, int maxAlloc1 = 0) : allocBlocks(allocBlocks1), allocMemory(allocMemory1), maxAlloc(maxAlloc1) {}
};

class MemoryStorage
{
public:
	explicit MemoryStorage(int size);
	~MemoryStorage();
	int allocate(int size);
	int free(int offset);
	MemInfo getInfo();
	std::string toString();

private:
	int mySize;
	Block* firstBlock;
};

#endif