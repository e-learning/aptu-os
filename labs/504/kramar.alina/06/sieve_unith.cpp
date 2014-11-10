#include <iostream>
#include <cstdint>
#include <limits>
#include <chrono>
#include <vector>
#include <string>

std::vector<char> S;
uint64_t n;
bool print_mode = false;

void parse_cmd(int argc, char **argv)
{
    const std::string debug_flag = "-p";
    switch (argc)
    {
        case 1:
            std::cout << "Usage: " << argv[0] << " [-p] [limit number]" << std::endl;
            exit(1);
            break;
        case 2:
            if (argv[1] == debug_flag)
            {
                std::cout << "Maybe you forgot the limit number? Please, restart the app" << std::endl;
                exit(1);
            }
            n = std::stoi(argv[1]);
            if (n <= 2)
            {
                std::cout << "There aren't prime numbers less 2. Sorry" << std::endl;
                exit(1);
            }
            break;
        case 3:
            if (argv[1] != debug_flag)
            {
                std::cout << "Too many params. I don't undestand you." << std::endl;
                exit(1);
            }
            n = std::stoi(argv[2]);
            print_mode = true;
            if (n <= 2)
            {
                std::cout << "Not a correct number" << std::endl;
                exit(1);
            }
            break;
        default:
            std::cout << "There aren't prime numbers less 2. Sorry" << std::endl;
    }
}

void sieve()
{
    for (uint64_t i = 2; i * i < n; ++i)
    {
        if (S[i])
        {
            for (uint64_t j = i * i; j < n; j += i)
            {
                S[j] = 0;
            }
        }
    }

}

int main(int argc, char **argv)
{
    parse_cmd(argc, argv);
    S = std::vector<char>(n);
    S[0] = 0;
    S[1] = 0;
    fill(S.begin() + 2, S.end(), 1);
    std::chrono::high_resolution_clock clock;

    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();
    sieve();
    std::chrono::time_point<std::chrono::high_resolution_clock> finish = clock.now();

    if (print_mode)
    {
        for (uint64_t i = 0; i < n; ++i)
        {
            if (S[i] == 1)
                std::cout << i << std::endl;
        }
    }
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << std::endl;

    return 0;
}
