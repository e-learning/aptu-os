#include <iostream>
#include <stdint.h>
#include <cstring>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return -1;
	}
	bool print = false;
	uint64_t n;
	if (!strcmp("-p", argv[1])) {
		print = true;
		if (argc < 3)
		{
			return -1;
		}
		n = atoi(argv[2]);
	}
	else
	{
		n = atoi(argv[1]);
	}
	
	bool *arr = new bool[(size_t)n + 1];
	memset(arr, true, sizeof(bool)* ((size_t)n + 1));

	for (uint64_t i = 2; ((i*i) <= n); ++i)
	{
		if (!arr[i])
		{
			continue;
		}
		for (uint64_t j = (i*i); j <= n; j += i)
		{
			arr[j] = false;
		}
	}
	if (!print)
	{
		delete[] arr;
		return 0;
	}
	for (uint64_t i = 1; i <= n; ++i)
	{
		if (arr[i])
		{
			std::cout << i << '\n';
		}
	}

	delete[] arr;
	return 0;
}
