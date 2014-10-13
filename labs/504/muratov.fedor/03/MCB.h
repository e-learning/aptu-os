#pragma once
struct MCB
{
	unsigned int size;
	MCB* next;
	MCB* prev;
	bool freeTag;
	unsigned startIndex;
};