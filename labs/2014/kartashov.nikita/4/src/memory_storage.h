#ifndef MEMORY_STORAGE_H
#define MEMORY_STORAGE_H

#include <string>
#include <iostream>

#include "block.h"

using namespace std;

using std::string;

const int FAIL = -1;
const int SUCCESS = 0;

struct memory_info
{
	memory_info() : allocated_blocks(0), allocated_memory(0), maximum_allocatable(0) {}

	int allocated_blocks;
	int allocated_memory;
	int maximum_allocatable;
};

class memory_storage
{
public:
	memory_storage(int size) : 
		m_size(size), 
		m_first_block(new block(m_size - HEADER_SIZE)),
		m_chunk(new char[m_size]) {}

	int allocate(int size)
	{
		block* current = m_first_block;
		int offset = 0;
		while (true)
		{
			if (current->is_free() && current->size() >= size)
			{
				if (current->size() - size >= MIN_BLOCK_SIZE)
				{
					block* free_block = new block(current->size() - size - HEADER_SIZE); // allocate block (size) and header for the free part (HEADER_SIZE)
					current->set_size(size);
					free_block->set_prev(current);
					free_block->set_next(current->next());
					if (current->next() != NO_BLOCK)
						current->next()->set_prev(free_block);
					current->set_next(free_block);
				}
				current->allocate();				
				return offset + HEADER_SIZE;
			}
			if (current->next() == NO_BLOCK)
				return FAIL;
			offset += HEADER_SIZE + current->real_size();
			current = current->next();
		}
	}

	int free(int offset)
	{
		int current_offset = 0;
		block* current = m_first_block;
		while (true) 
		{
			if (!current->is_free() && current_offset + HEADER_SIZE == offset)
			{
				current->free();
				current->merge();
				current->prev_merge();
				return SUCCESS;
			}
			if (current->next() == NO_BLOCK)
				return FAIL;
			offset += HEADER_SIZE + current->real_size();
			current = current->next();			
		}
	}

	memory_info info()
	{
		memory_info inf;
		block* current = m_first_block;
		while (current != NO_BLOCK)
		{
			if (!current->is_free())
			{
				++inf.allocated_blocks;
				inf.allocated_memory += current->real_size();
			}
			else
			{
				inf.maximum_allocatable = std::max(inf.maximum_allocatable, current->size());
			}
			current = current->next();
		}
		return inf;
	}

	string map()
	{
		string result(m_size, 'f');
		int offset = 0;
		block* current = m_first_block;
		while (current != NO_BLOCK)
		{
			pad_header(result, &offset);			
			char ch = current->is_free() ? 'f' : 'u';
			for (int i = 0; i < current->real_size(); ++i) 
			{
				result[offset++] = ch;
			}
			current = current->next();
		}
		return result;
	}

private:
	void pad_header(string& s, int* offset)
	{
		for (int i = 0; i < HEADER_SIZE; ++i) 
		{
			s[(*offset)++] = 'm';
		}
	}

	int m_size;
	block* m_first_block;
	char* m_chunk;
};

#endif /* end of include guard */
