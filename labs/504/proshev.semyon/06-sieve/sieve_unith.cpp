#include <vector>
#include <iostream>
#include <stdint.h>
#include <string>

void run_sieve(std::vector<bool>& result, const uint64_t n) {
    for (uint64_t i = 2; i * i < n; ++i) {
        if (result[i]) {
            for (uint64_t j = i + i; j < n; j += i) {
                result[j] = false;
            }
        }
    }
}

int main(int argc, char** argv) {
    bool print = false;
    uint64_t n;

    if (argc == 2) {
        n = std::stoi(argv[1]);
    } else if (argc == 3) {
        print = true;
        n = std::stoi(argv[2]);
    } else {
        std::cout << "Usage: [-p] N" << std::endl;
        return 0;
    }

    std::vector<bool> result = std::vector<bool>(n, true);

    run_sieve(result, n);

    if (print) {
    	for (uint64_t i = 2; i < n; ++i) {
    		if (result[i]) {
                std::cout << i << std::endl;
    		}
    	}
    }

    return 0;
}