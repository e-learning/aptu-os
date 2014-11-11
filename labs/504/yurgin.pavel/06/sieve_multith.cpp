#include <cmath>
#include <thread>
#include <atomic>
#include <chrono>
#include <ostream>
#include <iostream>
#include <string>
#include "sieve_multith.h"

std::vector<char> numbers;
std::atomic<ullong> sieve_i;

std::ostream& operator<<(std::ostream& os, std::vector<char> vec)
{
    for (ullong i = 0; i < vec.size(); ++i)
    {
        if (vec[i])
        {
            os << i << " ";
        }
    }
    os << std::endl;
    return os;
}


void sieve_mulith(ullong n)
{
    for (ullong i = 0; i * i < n;)
    {
        i = sieve_i.fetch_add(2);
        if (numbers[i] == 1)
        {
            ullong p = i * i;
            while (p < n)
            {
                numbers[p] = 0;
                p += i;
            }
        }
    }
}

int main(int argc, char ** argv)
{
    ullong n;
    size_t thread_numbers;
    std::string out_flag = "-p";
    bool out = false;

    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <out prime numbers flag> <max number> <thread number>"
                << std::endl;
        exit(1);
    }
    else if (argc == 3)
    {
        n = std::stoull(argv[1]);
        if (n <= 2)
        {
            std::cout << "Primes number not found" << std::endl;
            exit(0);
        }
        thread_numbers = std::stoi(argv[2]);
        if (thread_numbers <= 0)
        {
            std::cout << " Wrong threads number!" << std::endl;
            exit(1);
        }
    }
    else if (argc == 4)
    {
        if (argv[1] != out_flag)
        {
            std::cout << "Wrong parameters" << std::endl;
            std::cout << "Usage: " << argv[0] << " <out prime numbers flag> <max number>"
                    << std::endl;
            exit(1);
        }
        out = true;
        n = std::stoull(argv[2]);
        if (n <= 2)
        {
            std::cout << "Primes number not found" << std::endl;
            exit(0);
        }
        thread_numbers = std::stoi(argv[3]);
        if (thread_numbers <= 0)
        {
            std::cout << "Wrong number of threads" << std::endl;
            exit(1);
        }
    }
    if (argc > 4)
    {
        std::cout << "Wrong number of input parameters" << std::endl;
        std::cout << "Usage: " << argv[0] << " <out prime numbers flag> <max number>"
                << std::endl;
        exit(1);
    }

    numbers.reserve(n);
    numbers.assign(n, 1);
    numbers[0] = 0;
    numbers[1] = 0;
    for (ullong i = 4; i < n; i += 2)
    {
        numbers[i] = 0;
    }
    sieve_i.store(3);
    //std::chrono::high_resolution_clock clock;

    //std::chrono::time_point<std::chrono::high_resolution_clock> begin = clock.now();
    std::vector<std::thread> threads(thread_numbers);
    for (size_t i = 0; i < thread_numbers; ++i)
    {
        threads[i] = std::thread(sieve_mulith, n);
    }
    for (size_t i = 0; i < thread_numbers; ++i)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
    //std::chrono::time_point<std::chrono::high_resolution_clock > end = clock.now();
    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
    if (out)
    {
        std::cout << numbers;
    }
    return 0;
}
