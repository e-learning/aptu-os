#include <iostream>
#include <malloc.h>
#include <string.h>
#include <chrono>
#include <atomic>
#include <stdlib.h>
#include <bits/stl_algo.h>
#include <unistd.h>
#include <complex>

using namespace std;

bool *prime;

void init_prime(uint64_t n) {
    memset(prime, true, (n+1) * sizeof(*prime));
    prime[0] = prime[1] = false;
}

// Single Thread
void sieve_single_th(uint64_t n) {
    init_prime(n);

    for (uint64_t i = 2; i * i <= n; ++i)   // valid for n < 46340^2 = 2147395600
        if (prime[i])
            for (uint64_t j = i * i; j <= n; j += i)
                prime[j] = false;
}

// Multithread

struct info {
    uint64_t count;
    std::atomic<uint64_t> current;
};


void *sieve_multi_th(void *pdata) {
    info *inf = (info *) pdata;
    while (true) {
        uint64_t n = inf->count;
        uint64_t my_i = inf->current.fetch_add(1);
        if (my_i * my_i > n) {
            return NULL;
        }

        if (prime[my_i])
            for (uint64_t j = my_i * my_i; j <= n; j += my_i)
                prime[j] = false;
    }
}

void sieve_mt(int64_t N, int TC) {
    init_prime(N);

    pthread_t *threads = (pthread_t *) malloc(TC * sizeof(*threads));
    info info;
    info.count = N;
    info.current.store(2);
    for (int i = 0; i < TC; ++i) {
        pthread_create(&threads[i], 0, sieve_multi_th, &info);
    }

    for (int i = 0; i < TC; ++i) {
        pthread_join(threads[i], 0);
    }
    free(threads);
}

// Main

void testIt(uint64_t n) {
    int count = 0;
    for (int i = 0; i < n; ++i) {
        if (prime[i]) {
            cout << i << '\n';
            count++;
        }
    }
//    cout << "Count=" << count << '\n';
}

bool is_numeric(char *str) {
    size_t len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

void print_usage() {
    cout << "./06_threads N [M -p -i10]" << endl;
    cout << "N - ограничение решета" << endl;
    cout << "M - кол-во тредов" << endl;
    cout << "-p - печать простые числа" << endl;
    cout << "-i - количество итераций бенчмарка" << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
    }

    int64_t N = -1;
    bool single_thread = true;
    int TC = -1;

    for (int i = 0; i < argc; ++i) {
        char *arg = argv[i];
        if (is_numeric(arg)) {
            if (N == -1) {
                N = atoi(arg);
            } else if (TC == -1) {
                single_thread = false;
                TC = atoi(arg);
            }
        }
    }

    bool print_result = false;
    int iterations = 1;
    int c;
    while ((c = getopt(argc, argv, ":pi:")) != -1) {
        switch (c) {
            case 'p':
                print_result = true;
                break;
            case 'i':
                iterations = atoi(optarg);
                break;
            default:
                break;
        }
    }

    prime = (bool *) malloc((N+1) * sizeof(*prime));

    auto start = chrono::high_resolution_clock::now();
    if (single_thread) {
        for (int i = 0; i < iterations; i++) {
            sieve_single_th(N);
        }
    } else {
        for (int i = 0; i < iterations; i++) {
            sieve_mt(N, TC);
        }
    }
    auto finish = chrono::high_resolution_clock::now();
    if(print_result) {
        testIt(N);
    }
    cout << chrono::duration_cast<chrono::microseconds>(finish - start).count() / iterations << " mks\n";
    if(prime)
        free(prime);
    return 0;
}
