/*
 * allocator.cpp
 *
 *  Created on: 15 апр. 2014 г.
 *      Author: stani_000
 */
#include "allocator.h"
#include <iostream>
#include <algorithm>
using namespace std;

Allocator::Allocator(size_t size) {
	memory = new unsigned char[size];
	memory_size = size;
	Block* block = get_block(0);
	block->is_free = true;
	block->offset = 0;
	block->size = memory_size;
}

void Allocator::alloc(size_t size) {
	for (Block * i = get_block(0); i; i = get_next(i)) {
		if (i->is_free && i->usr_memory_size() >= size) {
			if (i->usr_memory_size() - size < sizeof(Block)) {
				i->is_free = false;
				cout << '+' << i->offset + sizeof(Block) << std::endl;
				return;
			} else {
				Block * next = get_block(i->usr_memory_offset() + size);
				next->prev = i->offset;
				next->offset = i->usr_memory_offset() + size;
				next->size = i->size - (next->offset - i->offset);
				next->is_free = true;
				i->is_free = false;
				i->size = next->offset - i->offset;
				cout << '+' << i->offset + sizeof(Block) << std::endl;
				return;
			}
		}
	}
	cout << '-' << std::endl;
}

void Allocator::free(size_t offset) {
	offset -= sizeof(Block);
	Block * block = get_block(offset);
	block->is_free = true;
	if (has_prev(block) && has_next(block) && get_prev(block)->is_free
			&& get_next(block)->is_free) {
		if (has_next(get_next(block))) {
			get_next(get_next(block))->prev = get_prev(block)->offset;
		}
		get_prev(block)->size += (block->size + get_next(block)->size);
	} else if (has_prev(block) && get_prev(block)->is_free) {
		if (has_next(block)) {
			get_next(block)->prev = get_prev(block)->offset;
		}
		get_prev(block)->size += block->size;
	} else if (has_next(block) && get_next(block)->is_free) {
		if (has_next(get_next(block))) {
			get_next(get_next(block))->prev = block->offset;
		}
		block->size += get_next(block)->size;
		block->is_free = true;
	}

	cout << '+' << std::endl;
}

void Allocator::info() {
	int usr_blocks = 0;
	int usr_mem = 0;
	size_t max_size = 0;

	for (Block * i = get_block(0); i; i = get_next(i)) {
		if (i->is_free) {
			max_size = std::max(max_size, i->usr_memory_size());
		} else {
			++usr_blocks;
			usr_mem += i->usr_memory_size();
		}
	}

	std::cout << usr_blocks << " " << usr_mem << " " << max_size << std::endl;
}

void Allocator::map() {
	for (Block * i = get_block(0); i; i = get_next(i)) {
		for (unsigned int j = 0; j < sizeof(Block); ++j) {
			std::cout << 'm';
		}

		if (i->is_free) {
			for (size_t j = 0; j < i->usr_memory_size(); ++j) {
				std::cout << 'f';
			}
		} else {
			for (size_t j = 0; j < i->usr_memory_size(); ++j) {
				std::cout << 'u';
			}
		}
	}
	std::cout << std::endl;
}

Block* Allocator::get_block(size_t offset) {

	return reinterpret_cast<Block *>(memory + offset);
}

Block* Allocator::get_next(Block* b) {
	if (b->offset + b->size < memory_size) {
		return get_block(b->offset + b->size);
	} else {
		return 0;
	}
}

Block* Allocator::get_prev(Block* curr) {
	if (curr->offset != 0) {
		return get_block(curr->prev);
	} else {
		return 0;
	}
}

bool Allocator::has_next(Block* b) {
	if (b->offset + b->size < memory_size) {
		return true;
	} else {
		return false;
	}
}

bool Allocator::has_prev(Block* b) {
	if (b->offset != 0) {
		return 1;
	} else {
		return 0;
	}

}

size_t Block::usr_memory_size() {
	return size - sizeof(Block);
}

size_t Block::usr_memory_offset() {
	return offset + sizeof(Block);
}

Block* get_block(size_t offset, size_t ptr) {
	return reinterpret_cast<Block *>(ptr + offset);
}

