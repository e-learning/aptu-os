//#pragma once
#include "MCB.h"
#include <tuple>
#include <string>

using namespace std;

class ToyAllocator
{
private:
	char* heap;
	MCB first_MCB;
	MCB last_MCB;
	unsigned user_used;
	unsigned max_heap_size;
	unsigned real_heap_size;

public:
	ToyAllocator(unsigned int heap_size);
	unsigned int allocate(unsigned int size);
	bool free(unsigned int pointer);
	tuple<unsigned, unsigned, unsigned> info();
	string map();
	~ToyAllocator();

private:
	static unsigned AlignSize(unsigned size);
	static	void fill_MCB(MCB* p_mcb,unsigned size, MCB* prev, MCB* next, bool freeFlag, unsigned startIndex);
	void writeMCBtoBlock(unsigned block, MCB* mcb_to_write);
	MCB* readMCBFromBlock(unsigned block);
};

