#include <iostream>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <stack>
#include <thread>

#include "bit_array.h"

using namespace std;

string usage = "arguments are:\n"
        "\tlimit of sieve - first number in arguments\n"
        "\tthread - second number in arguments\n"
        "\t-p - optional flag, specify for printing prime numbers\n";

uint64_t read_limit(const char *str)
{
    uint64_t limit;
    try
    {
        limit = stoull(str);
    }
    catch (...)
    {
        cerr << "Invalid argument: expected number, but recieved " << str << endl;
        exit(1);
    }
    return limit;
}

uint32_t read_thread_count(const char *str)
{
    uint32_t c;
    try
    {
        c = stoul(str);
    }
    catch (...)
    {
        cerr << "Invalid argument: expected number, but recieved " << str << endl;
        exit(1);
    }
    return c;
}


void threadf(bit_array &sieve, uint64_t left_bound, uint64_t right_bound)
{

    for (uint64_t j = 2; j * j <= right_bound; ++j)
    {
        if (!sieve.get(j))
        {
            for (uint64_t i = max((left_bound / j) * j, j * j); i <= right_bound; i += j)
            {
                if (i < left_bound)
                    continue;
                sieve.set(i, true);
            }
        }
    }
}

int main(int argc, const char **argv)
{
    if (argc == 1)
    {
        cout << usage;
        return 0;
    }

    bool print = false;
    uint64_t limit = 0;
    uint32_t threads_count = 0;


    bool limit_readed = false;
    bool threads_readed = false;
    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "-p")
            print = true;
        else if (!limit_readed)
        {
            limit = read_limit(argv[i]);
            limit_readed = true;
        }
        else
        {
            threads_count = read_thread_count(argv[i]);
            threads_readed = true;
        }
    }

    if (!limit_readed || !threads_readed)
    {
        cerr << "Some required arguments not speciefed" << endl;
        return 1;
    }

    if (threads_count == 0)
    {
        cerr << "Zero thread count is impossible" << endl;
        return 1;
    }

    bit_array *p_sieve;
    try
    {
        p_sieve = new bit_array(limit);
    }
    catch (...)
    {
        cerr << "Too large number: " << limit << endl;
        return 0;
    }


    bit_array& sieve = *p_sieve;

    sieve.set(0, true);
    sieve.set(1, true);

    stack<thread> active_thread;

    uint64_t count_to_thread = limit / threads_count;
    uint64_t left_bound = 3;
    uint64_t right_bound;
    for (uint32_t j = 1; j < threads_count; ++j)
    {
        left_bound  = (left_bound / 8) * 8;
        right_bound = min(((left_bound + count_to_thread) / 8 + 1) * 8, limit);
        cerr << left_bound << " "  << right_bound << endl;
        active_thread.push(thread(threadf, ref(sieve), left_bound, right_bound));
        left_bound  = right_bound;
    }
    active_thread.push(thread(threadf, ref(sieve), left_bound, limit));
    cerr << left_bound << " "  << limit << endl;
    while (!active_thread.empty())
    {
        thread &thr = active_thread.top();
        thr.join();
        active_thread.pop();
    }

    if (print)
    {
        for (uint64_t i = 2; i < limit; ++i)
        {
            if (!sieve.get(i))
            {
                cout << i << "\n";
            }
        }
    }


    return 0;
}

