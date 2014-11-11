#include <iostream>
#include <stdint.h>
#include <thread>
#include <time.h>
#include <cstring>
#include <stdlib.h>

void thread_function(bool *arr, uint64_t from, uint64_t to, uint64_t step)
{
	for (uint64_t k = from; k <= to; k += step)
	{
		arr[k] = false;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		return -1;
	}
	bool print = false;
	uint64_t n;
	int m;
	if (!strcmp("-p", argv[1])) {
		print = true;
		if (argc < 4)
		{
			return -1;
		}
		n = atoi(argv[2]);
		m = atoi(argv[3]);
	}
	else
	{
		n = atoi(argv[1]);
		m = atoi(argv[2]);
	}

	bool *arr = new bool[(size_t)n + 1];
	memset(arr, true, sizeof(bool)* ((size_t)n + 1));
    std::thread *threads = new std::thread[m];

	for (uint64_t i = 2; ((i*i) <= n); ++i)
	{
		if (!arr[i])
		{
			continue;
		}
		uint64_t from = i*i;
		uint64_t step = ((n - from + i - 1) / i / m) * i;
		for (int j = 0; j < m - 1; ++j)
		{
			// TODO reuse threads(thread pool or something)
			threads[j] = std::thread(thread_function, arr, from, from + step - 1, i);
			from += step;
		}
		threads[m - 1] = std::thread(thread_function, arr, from, n, i);

		for (int j = 0; j < m; ++j)
		{
			threads[j].join();
		}
	}
    delete[] threads;

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
