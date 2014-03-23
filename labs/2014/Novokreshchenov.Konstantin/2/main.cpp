#define _CRT_SECURE_NO_WARNINGS

#include "scheduler.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

int main()
{
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);

    size_t quantum;
	std::cin >> quantum;
	Scheduler scheduler(quantum);

	std::string line = "";
	while (std::getline(std::cin, line)) {
		if (! line.empty()) {
			scheduler.add(parse_process(line));
		}
	}

	int i = 1;
	while (! scheduler.isempty()) {
		if (i == 32) {
			i = 32;
		}
		++i;
		scheduler.next();
	}

	return 0;
}
