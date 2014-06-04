#include <iostream>
#include <string>

#include "memoryStorage.h"


int main()
{
	int n = 0;
	std::cin >> n;

	MemoryStorage storage(n);
	std::string buf;

	while (std::cin >> buf)
	{
		if (buf == "INFO")
		{
			MemInfo info = storage.getInfo();
			std::cout << info.allocBlocks << " " << info.allocMemory << " " << info.maxAlloc << std::endl;
			continue;
		}

		if (buf == "MAP")
		{
			std::cout << storage.toString() << std::endl;
			continue;
		}

		int bufint = 0;
		std::cin >> bufint;

		if (buf == "ALLOC")
		{
			int value = storage.allocate(bufint);
			if (value == -1)
			{
				std::cout << "-" << std::endl;
			} else
			{
				std::cout << "+ " << value << std::endl;
			}
		}
		else
		{
			int result = storage.free(bufint);

			if (result != 0)
			{
				std::cout << "-" << std::endl;
			} else
			{
				std::cout << "+" << std::endl;
			}
		}
	}
	return 0;
}
