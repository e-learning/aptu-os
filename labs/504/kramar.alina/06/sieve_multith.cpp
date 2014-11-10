#include <iostream>
#include <cstdint>
#include <limits>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

std::vector<char> S;
size_t thread_number;
uint64_t n;
std::atomic <uint64_t> current_number;
bool print_mode = false;

void parse_cmd(int argc, char **argv)
{
    const std::string debug_flag = "-p";
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " [-p] [limit number] [count of threads]"
                  << std::endl;
        exit(1);
    }
    if (argc == 3)
    {
        n = std::stoi(argv[1]);
        if (n <= 2)
        {
            std::cout << "Not a correct number" << std::endl;
            exit(1);
        }
        thread_number = std::stoi(argv[2]);
        if (thread_number == 0)
        {
            std::cout << "Not a correct number of threads" << std::endl;
            exit(1);
        }
        return;
    }
    if (argc == 4)
    {
        if (argv[1] != debug_flag)
        {
            std::cout << "Wrong parameters" << std::endl;
            exit(1);
        }
        print_mode = true;
        n = std::stoi(argv[2]);
        if (n <= 2)
        {
            std::cout << "Not a correct number" << std::endl;
            exit(1);
        }
        thread_number = std::stoi(argv[3]);
        if (thread_number == 0)
        {
            std::cout << "Not a correct number of threads" << std::endl;
            exit(1);
        }
        return;
    }
    if (argc > 4)
    {
        std::cout << "Wrong number of input parameters" << std::endl;
        exit(1);
    }
}

void sieve()
{
    while (true)
    {
        uint64_t N = n;
        uint64_t sieve_number = current_number.fetch_add(1);
        if (sieve_number * sieve_number >= N)
        {
            return;
        }
        if (S[sieve_number])
        {
            for (uint64_t j = sieve_number * sieve_number; j < n; j += sieve_number)
                S[j] = 0;
        }
    }
}


int main(int argc, char **argv)
{
    parse_cmd(argc, argv);
    current_number.store(2);
    S = std::vector<char>(n);
    S[0] = 0;
    S[1] = 0;
    fill(S.begin() + 2, S.end(), 1);
    std::chrono::high_resolution_clock clock;

    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();
    std::vector<std::thread> threads(thread_number);
    for (size_t i = 0; i < thread_number; ++i)
    {
        threads[i] = std::thread(sieve);
    }
    for (size_t i = 0; i < thread_number; ++i)
    {
        threads[i].join();
    }
    std::chrono::time_point<std::chrono::high_resolution_clock> finish = clock.now();

    if (print_mode)
    {
        for (u_int64_t i = 0; i < n; ++i)
        {
            if (S[i] == 1)
                std::cout << i << std::endl;
        }
    }

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << std::endl;

    return 0;
}
