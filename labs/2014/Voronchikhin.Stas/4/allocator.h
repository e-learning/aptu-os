/*
 * allocator.h
 *
 *  Created on: 15 апр. 2014 г.
 *      Author: stani_000
 */

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_
#include <vector>

using namespace std;
struct Block {
	size_t offset;
	size_t size;
	size_t prev;
	bool is_free;

	size_t usr_memory_size();
	size_t usr_memory_offset();

};

struct Allocator {

	unsigned char* memory;
	size_t memory_size;

	Allocator(size_t size);

	void alloc(size_t size);
	void free(size_t place);
	void info();
	void map();


	Block* get_block(size_t offset);
	Block* get_next(Block* curr);
	Block* get_prev(Block* curr);

	bool has_next(Block* b);
	bool has_prev(Block* b);

};

#endif /* ALLOCATOR_H_ */
