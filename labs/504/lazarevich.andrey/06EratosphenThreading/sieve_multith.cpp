#include <iostream>
#include <cstdint>
#include <limits>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

using namespace std;

vector<bool> S;
size_t thread_number;
u_int64_t n;
atomic <u_int64_t> current_number;
bool debug = false;


void parse_cmd(int argc, char **argv)
{
    const string debug_flag = "-p";
    if (argc < 3)
    {
        cout << "Usage: " << argv[0] << " <debug_flag> <limit number> <threads number>"
             << endl;
        exit(1);
    }
    if (argc == 3)
    {
        n = stoi(argv[1]);
        if (n <= 2)
        {
            cout << "Not a correct number" << endl;
            exit(1);
        }
        thread_number = stoi(argv[2]);
        if (thread_number == 0)
        {
            cout << "Not a correct number of threads" << endl;
            exit(1);
        }
        return;
    }
    if (argc == 4)
    {
        if (argv[1] != debug_flag)
        {
            cout << "Wrong parameters" << endl;
            exit(1);
        }
        debug = true;
        n = stoi(argv[2]);
        if (n <= 2)
        {
            cout << "Not a correct number" << endl;
            exit(1);
        }
        thread_number = stoi(argv[3]);
        if (thread_number == 0)
        {
            cout << "Not a correct number of threads" << endl;
            exit(1);
        }
        return;
    }
    if (argc > 4)
    {
        cout << "Wrong number of input parameters" << endl;
        exit(1);
    }
}

void sieve()
{
    while (true)
    {
        u_int64_t N = n;
        u_int64_t sieve_number = current_number.fetch_add(1);
        if (sieve_number * sieve_number >= N)
        {
            return;
        }
        if (S[sieve_number])
        {
            for (u_int64_t j = sieve_number * sieve_number; j < n; j += sieve_number)
                S[j] = false;
        }
    }
}


int main(int argc, char **argv)
{
    parse_cmd(argc, argv);
    current_number.store(2);
    S = vector<bool>(n);
    S[0] = false;
    S[1] = false;
    fill(S.begin() + 2, S.end(), true);
    vector<thread> threads(thread_number);
    chrono::high_resolution_clock clock;

    chrono::time_point<chrono::high_resolution_clock> start = clock.now();
    for (size_t i = 0; i < thread_number; ++i)
    {
        threads[i] = thread(sieve);
    }
    for (size_t i = 0; i < thread_number; ++i)
    {
        threads[i].join();
    }
    chrono::time_point<chrono::high_resolution_clock> finish = clock.now();

    if (debug)
    {
        for (u_int64_t i = 0; i < n; ++i)
        {
            if (S[i] == 1)
                cout << i << endl;
        }
    }

    cout << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << endl;

    return 0;
}

