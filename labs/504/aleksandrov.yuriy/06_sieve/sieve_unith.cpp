#include <iostream>
#include <fstream>
#include <cinttypes>
#include <vector>
#include <cmath>
#include <ctime>

#include "texceptions.hpp"
#include "get_params.hpp"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;


typedef vector<bool> TestingNumbers;

TestingNumbers sieve_of_eratosthenes(uint64_t n)
{
    TestingNumbers is_prime(n-2, true);
    uint64_t maxi = sqrt(n);
    for (uint64_t i = 3; i <= maxi; i += 2)
    {
        if (is_prime[i-2])
        {
            for (uint64_t j = i*i; j < n; j += i)
            {
                is_prime[j-2] = false;
            }
        }
    }
    
    return is_prime;
}


int main(int argc, char* argv[])
{
    clock_t start_time = clock();
    
    Params params;
    try
    {
        params =  get_params(argc, argv, false);
        TestingNumbers is_prime = sieve_of_eratosthenes(params.N);
        if (params.is_print && params.N >= 2)
        {
            cout << 2;
            for (uint64_t i = 1; i < is_prime.size(); ++i)
                if (is_prime[i])
                    cout << '\n' << i+2;
            cout << endl;
        }
    }
    catch (TException exc)
    {
        cerr << exc.what() << endl;
    }
    catch (std::bad_alloc& ba)
    {
        cerr << "Error: not enough memory available" << endl;
    }
    
    clock_t end_time = clock();
    ofstream time_file("sieve_unith.time");
    time_file << 1000.*(end_time - start_time) / CLOCKS_PER_SEC << " ms" << endl;
    time_file.close();
    
    return 0;
}
