#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <cinttypes>
#include <vector>
#include <cmath>
#include "get_params.hpp"
#include "texceptions.hpp"


using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::vector;
using std::thread;

typedef vector<bool> TestingNumbers;

template<typename T>
std::ostream & operator<<(std::ostream &output, const std::vector<T> & v)
{
    output << "{ ";
    for (auto iter = v.begin(); iter != v.end(); ++iter)
    {
        output << *iter << ' ';
    }
    output << "}";
    return output;
}

void sieve_of_eratosthenes(TestingNumbers & is_prime, std::atomic<uint64_t> & curr_number, uint32_t th_ind)
{ 
    uint64_t max_num = is_prime.size()+3;
    uint64_t max_sqrt_num = sqrt(max_num);
    uint64_t div;
    //cerr << "THREAD: " << th_ind << '\n';
    while ((div = curr_number.fetch_add(2)) <= max_sqrt_num)
    {
        //cerr << " div = " << div << " and it is_prime = " << (is_prime[div-3] ? "true" : "false") << '\n';
        if (is_prime[div-3])
        {
            for (uint64_t j = div*div; j < max_num; j += div)
            {
                is_prime[j-3] = false;
                //cerr <<  "   " << j << "-3 is false" << '\n'; 
            }
        }
    }
}


int main(int argc, char* argv[])
{
    clock_t start_time = clock();
    
    Params params;
    try
    {
        params =  get_params(argc, argv, true);
        
        if ( params.N > 2)
        {
            std::atomic<uint64_t> curr_number(3);
            TestingNumbers is_prime(params.N-3, true); // 3, 4, 5, ...
            thread* threads = new thread[params.threads_num];
            
            for (uint32_t th = 0; th < params.threads_num; ++th)
            {
                threads[th] = thread(sieve_of_eratosthenes, std::ref(is_prime), std::ref(curr_number), th);
            }
            for (uint32_t th = 0; th < params.threads_num; ++th)
            {
                threads[th].join();
            }
                    
            if (params.is_print)
            {
                cout << 2;
                for (uint64_t i = 0; i < is_prime.size(); i += 2)
                    if (is_prime[i])
                        cout << '\n' << i+3;
                cout << endl;
            }
            
            delete [] threads;
        }
        else if ( params.N == 2 && params.is_print)
        {
            cout << 2 << endl; 
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
    ofstream time_file("sieve_multith.time");
    time_file << 1000.*(end_time - start_time) / CLOCKS_PER_SEC << "ms" << endl;
    time_file.close();
    
    return 0;
}
