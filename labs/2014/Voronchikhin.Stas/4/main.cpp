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
			int size;
			cin >> size;
			all->alloc(size);
		} else if (s == "FREE") {
			int place;
			cin >> place;
			all->free(place);
		} else if (s == "INFO") {
			all->info();
		} else if (s == "MAP") {
			all->map();
		} else if (s == "PRINT") {
			all->print();
		}
	}
	cout.flush();
	return 0;
}

