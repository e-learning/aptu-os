#include <iostream>
#include <stdint.h>
#include <thread>
#include <time.h>
#include <cstring>
#include <stdlib.h>
#include <atomic>

void thread_function(bool *arr, std::atomic_uint_least64_t &index, u_int64_t n)
{
    while(index < n)
    {
        u_int64_t i = index.fetch_add(1);
        if(!arr[i])
        {
            continue;
        }
        uint64_t k = i * i;
        if(k > n)
        {
            return;
        }
        for (; k <= n; k += i)
        {
            arr[k] = false;
        }
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
    std::atomic_uint_least64_t index(2);

    for(int i = 0; i < m; ++i)
    {
        threads[i] = std::thread(thread_function, arr, std::ref(index), n);
    }
    for(int i = 0; i < m; ++i)
    {
        threads[i].join();
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
