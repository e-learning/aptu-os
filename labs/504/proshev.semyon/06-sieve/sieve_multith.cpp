#include <vector>
#include <iostream>
#include <stdint.h>
#include <string>
#include <thread>
#include <atomic>

uint64_t n;
std::vector<char> result;
std::atomic<uint64_t> point;

void run_sieve() {
    while (true) {
        uint64_t i = point.fetch_add(1);

        if (i * i >= n) {
            return;
        }

        if (result[i]) {
            for (uint64_t j = i + i; j < n; j += i) {
                result[j] = false;
            }
        }
    }
}

int main(int argc, char** argv) {
    bool print = false;
    uint64_t m;

    if (argc == 3) {
        n = std::stoi(argv[1]);
        m = std::stoi(argv[2]);
    } else if (argc == 4) {
        print = true;
        n = std::stoi(argv[2]);
        m = std::stoi(argv[3]);
    } else {
        std::cout << "Usage: [-p] N M" << std::endl;
        return 0;
    }

    point.store(2);

    result = std::vector<char>(n, true);

    std::vector<std::thread> threads(m);

    for (size_t i = 0; i < m; ++i)
    {
        threads[i] = std::thread(run_sieve);
    }
    for (size_t i = 0; i < m; ++i)
    {
        threads[i].join();
    }

    if (print) {
        for (uint64_t i = 2; i < n; ++i) {
            if (result[i]) {
                std::cout << i << std::endl;
            }
        }
    }

    return 0;
}