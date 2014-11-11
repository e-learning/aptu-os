#include <cmath>
#include <chrono>
#include <iostream>
#include "sieve_unith.h"
#include <string>

std::vector<char> numbers;

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

void sieve_unith(ullong n)
{

    for (ullong i = 3; i * i < n; i += 2)
    {
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
    bool out = false;
    const std::string out_flag = "-p";

    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <out prime numbers flag> <max number>"
                << std::endl;
        exit(1);
    }
    else if (argc == 2)
    {
        n = std::stoull(argv[1]);
        if (n <= 2)
        {
            std::cout << "Primes number not found" << std::endl;
            exit(0);
        }
    }
    else if (argc == 3)
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
    }
    if (argc > 3)
    {
        std::cout << "Wrong number of input parameters" << std::endl;
        std::cout << "Usage: " << argv[0] << " <out prime numbers flag> <max number>"
                << std::endl;
        exit(1);
    }


    numbers.assign(n, 1);
    numbers[0] = 0;
    numbers[1] = 0;
    for (ullong i = 4; i < n; i += 2)
    {
        numbers[i] = 0;
    }

    //std::chrono::high_resolution_clock clock;
    //std::chrono::time_point<std::chrono::high_resolution_clock > begin = clock.now();

    sieve_unith(n);

    //std::chrono::time_point<std::chrono::high_resolution_clock > end = clock.now();
    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;

    if (out) {
        std::cout << numbers;

    }
    return 0;
}

