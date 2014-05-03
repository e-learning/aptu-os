#include <iostream>

#include "memory.h"

int main()
{
	std::string input;
	std::cin >> input;
	memory mem;
	mem.init_from_stdin();
	std::cout << mem.physical_address(input) << std::endl;
	return 0;
}
