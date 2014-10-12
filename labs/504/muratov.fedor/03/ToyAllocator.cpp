#include "stdafx.h"
#include "ToyAllocator.h"
//#include <new>

ToyAllocator::ToyAllocator(unsigned int heap_size)
{
	max_heap_size = heap_size;
	user_used = 0;

	real_heap_size =  AlignSize(heap_size);
	//real_heap_size = heap_size;
	heap = new char[real_heap_size];

	fill_MCB(&first_MCB, real_heap_size, nullptr, &last_MCB, true, 0);
	fill_MCB(&last_MCB, 0, &first_MCB, nullptr, false, real_heap_size + 1);
}

unsigned ToyAllocator::allocate(unsigned size)
{
	size = AlignSize(size) - sizeof(MCB);
	MCB *current = &first_MCB;
	if (max_heap_size < size + user_used)
	{
		throw bad_alloc();
	}
	while (current->next != NULL)
	{
		if (current->freeTag == true && current->size >= size + sizeof(MCB))
		{
			user_used += size;
			MCB newNode;
			int prev_idex;
			if (current->prev != NULL)
			{
				prev_idex = current->startIndex;
			}
			else
			{
				prev_idex = 0;
			}

			fill_MCB(&newNode, current->size - size - sizeof(MCB), current, current->next, true, prev_idex + size + sizeof(MCB));
			writeMCBtoBlock(current->startIndex + size, &newNode);
			current->size = size;
			current->freeTag = false;
			current->next = readMCBFromBlock(newNode.startIndex - sizeof(MCB));
			return current->startIndex;
		}
		current = current->next;
	}
	throw bad_alloc();
}

bool ToyAllocator::free(unsigned pointer)
{
	MCB* current = &first_MCB;
	while (current->next != NULL)
	{
		if (current->startIndex == pointer && current->freeTag == false)
		{
			bool capturePrev = false;
			bool captureNext = false;
			user_used -= current->size;
			if (current->prev != NULL && current->prev->freeTag == true)
			{
				capturePrev = true;
			}

			if (current->next->freeTag == true)
			{
				captureNext = true;
			}

			if (captureNext && capturePrev)
			{
				current->prev->size += current->size + current->next->size + 2 * sizeof(MCB);
				current->prev->next = current->next->next;
				return true;
			}

			if (capturePrev)
			{
				current->prev->size += current->size + sizeof(MCB);
				current->prev->next = current->next;
				return true;
			}

			if (captureNext)
			{
				current->freeTag = true;
				current->size += current->next->size + sizeof(MCB);
				current->next = current->next->next;
				return true;
			}
			
			current->freeTag = true;
			return true;
		}
		current = current->next;
	}
	return false;
}

void ToyAllocator::fill_MCB(MCB* p_mcb ,unsigned size, MCB* prev, MCB* next, bool freeFlag, unsigned startIndex)
{
	p_mcb->freeTag = freeFlag;
	p_mcb->prev = prev;
	p_mcb->next = next;
	p_mcb->size = size;
	p_mcb->startIndex = startIndex;
}

unsigned ToyAllocator::AlignSize(unsigned size)
{
	unsigned offset = size % sizeof(MCB);
	if (offset > 0)
		offset = sizeof(MCB)-offset;
	return size + offset + sizeof(MCB);
}

void ToyAllocator::writeMCBtoBlock(unsigned block, MCB* mcb_to_write)
{
	*reinterpret_cast<MCB*>(heap + block) = *mcb_to_write;
}

MCB* ToyAllocator::readMCBFromBlock(unsigned block)
{
	return reinterpret_cast<MCB*>(heap + block);
}

string ToyAllocator::map()
{
	string map(real_heap_size, '0');
	MCB current = first_MCB;
	while (current.next != NULL)
	{
		char symbol;
		if (current.freeTag)
		{
			symbol = 'f';
		}
		else
		{
			symbol = 'u';
		}
		if (current.prev != NULL && current.next != NULL)
		{
			map.replace(current.startIndex - sizeof(MCB), current.startIndex, string(sizeof(MCB), 'm'));
		}
		map.replace(current.startIndex, current.startIndex + current.size, string(current.size, symbol));
		current = *current.next;
	}
	return map;
}

tuple<unsigned, unsigned, unsigned> ToyAllocator::info()
{
	unsigned userBlocks = 0;
	unsigned userAllocated = 0;
	unsigned maxPossibleAlloc = max_heap_size;
	MCB current = first_MCB;
	while (current.next != NULL)
	{
		if (current.freeTag == false)
		{
			userBlocks++;
			userAllocated += current.size;
			maxPossibleAlloc -= current.size;
		}
		current = *current.next;
	}
	if (maxPossibleAlloc < sizeof(MCB))
	{
		maxPossibleAlloc = 0;
	}
	if (maxPossibleAlloc % sizeof(MCB) != 0)
	{
		maxPossibleAlloc = AlignSize(maxPossibleAlloc) - 2 * sizeof(MCB);
	}
	return make_tuple(userBlocks, userAllocated, maxPossibleAlloc);
}

ToyAllocator::~ToyAllocator()
{
	delete[] heap;
}
