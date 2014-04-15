/*
 * allocator.cpp
 *
 *  Created on: 15 апр. 2014 г.
 *      Author: stani_000
 */
#include "allocator.h"
#include <iostream>

using namespace std;

Allocator::Allocator(int size) {
	memory.resize(size);
	std::fill(memory.begin(), memory.end(), 0);
}

void Allocator::alloc(int size) {
	if (size > 0) {
		for (size_t i = 0; i < memory.size(); ++i) {
			if (memory[i] == 0) {
				size_t j = 1;
				for (; j <= size; ++j && j < memory.size() - i) {
					if (memory[i + j] != 0)
						break;
				}
				if (--j == size) {
					memory[i] = -size;
					for (int j = 1; j <= size; ++j) {
						memory[i + j] = 1;
					}
					cout << "+ " << i << std::endl;
					return;
				}
			} else if (memory[i] < 0) {
				i += -memory[i];
			}
		}
	}
	cout << "-" << std::endl;

}

void Allocator::free(int place) {
	if (memory[place] < 0) {
		int size = -memory[place];
		for (int i = 0; i <= size; ++i) {
			memory[place + i] = 0;
		}
		cout << "+" << std::endl;
	} else {
		cout << "-" << std::endl;
	}
}

void Allocator::info() {
	int max = 1;
	int sum = 0;
	int current_chunk = 0;
	size_t counter = 0;
	for (size_t i = 0; i < memory.size(); ++i) {
		if (memory[i] < 0) {
			int size = -memory[i];
			sum += size;
			++counter;
			if (current_chunk > max) {
				max = current_chunk;
			}
			current_chunk = 0;
			i += size;
		} else {
			++current_chunk;
		}

	}

	if (current_chunk > max) {
		max = current_chunk;
	}

	cout << counter << " " << sum << " " << max - 1 << std::endl;
}

void Allocator::map() {
	for (size_t i = 0; i < memory.size(); ++i) {
		if (memory[i] > 0) {
			cout << "u";
			continue;
		}
		if (memory[i] < 0) {
			cout << "m";
			continue;
		}
		cout << "f";
	}

	cout << endl;
}

void Allocator::print() {
	for (size_t i = 0; i < memory.size(); ++i) {
		cout << memory[i] << " ";
	}
	cout << std::endl;
}

