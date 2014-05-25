/*
 * main.cpp
 *
 *  Created on: 15 апр. 2014 г.
 *      Author: stani_000
 */
#include <iostream>
#include <string>
#include "allocator.h"

using namespace std;
int main(int argc, char **argv) {
	int size;
	cin >> size;
	Allocator* all = new Allocator(size);
	string s = "";
	while (true) {
		cin >> s;
		if (s == "ALLOC") {
			size_t size;
			cin >> size;
			all->alloc(size);
		} else if (s == "FREE") {
			size_t place;
			cin >> place;
			try {
				all->free(place);
			} catch (...) {
				cout << '-' << std::endl;
			}
		} else if (s == "INFO") {
			all->info();
		} else if (s == "MAP") {
			all->map();
		}
	}
	cout.flush();
	return 0;
}

