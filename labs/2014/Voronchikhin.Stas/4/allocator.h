/*
 * allocator.h
 *
 *  Created on: 15 апр. 2014 г.
 *      Author: stani_000
 */

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <vector>
struct Allocator {

	std::vector<int> memory;

	Allocator(int size);

	void alloc(int size);
	void free(int place);
	void info();
	void map();
	void print();
};

#endif /* ALLOCATOR_H_ */
