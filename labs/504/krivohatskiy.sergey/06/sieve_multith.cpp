#include <iostream>
#include <stdint.h>
#include <thread>
#include <time.h>
#include <cstring>
#include <stdlib.h>
#include <atomic>
#include <memory>

void thread_function(bool *arr, std::atomic_uint_least64_t &index, u_int64_t n, u_int64_t *thread_used_nums, int m, int cur)
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
        thread_used_nums[cur] = i;

        // check for collisions
        bool con = false;
        for(int j = 0; j < m; ++j)
        {
            if(thread_used_nums[j] >= i)
            {
                continue;
            }
            if(i % thread_used_nums[j] == 0)
            {
                // collision, exit
                con = true;
            }
        }
        if(con)
        {
            continue;
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

    std::unique_ptr<bool[]> arr(new bool[(size_t)n + 1]);
    memset(arr.get(), true, sizeof(bool)* ((size_t)n + 1));
    std::unique_ptr<std::thread[]> threads(new std::thread[m]);
    std::unique_ptr<u_int64_t[]> thread_used_nums(new u_int64_t[m]);
    std::atomic_uint_least64_t index(2);

    for(int i = 0; i < m; ++i)
    {
        thread_used_nums[i] = 2;
    }

    for(int i = 0; i < m; ++i)
    {
        threads[i] = std::thread(thread_function, arr.get(), std::ref(index), n, thread_used_nums.get(), m, i);
    }
    for(int i = 0; i < m; ++i)
    {
        threads[i].join();
    }

	if (!print)
    {
        return 0;
	}

    for (uint64_t i = 2; i <= n; ++i)
	{
		if (arr[i])
		{
			std::cout << i << '\n';
		}
	}


	return 0;
}
