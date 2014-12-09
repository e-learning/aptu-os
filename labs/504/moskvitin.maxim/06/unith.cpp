#include <iostream>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <ctime>
#include <algorithm>

#include "bit_array.h"

using namespace std;

string usage = "arguments are:\n"
        "\tlimit of sieve\n"
        "\t-p - optional flag, specify for printing prime numbers\n";

int main(int argc, const char **argv)
{
    if (argc == 1)
    {
        cout << usage;
        return 0;
    }

    bool print = false;
    uint64_t limit = 0;
    bool limit_readed = false;
    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "-p")
            print = true;
        else
        {
            try
            {
                limit = stoull(argv[i]);
                limit_readed = true;
            }
            catch (...)
            {
                cerr << "Invalid argument: expected number, but recieved " << argv[i] << endl;
                return 1;
            }
        }
    }

    if (!limit_readed)
    {
        cerr << "Limit not speciefed" << endl;
        return 1;
    }


    bit_array *p_sieve;
    try
    {
        p_sieve = new bit_array(limit);
    }
    catch (...)
    {
        cerr << "Too large number: " << limit << endl;
        return 1;
    }

    bit_array& sieve = *p_sieve;

    sieve.set(0, true);
    sieve.set(1, true);


    for (uint64_t i = 2; i * i < limit; ++i)
    {
        if (!sieve.get(i))
        {
            for (uint64_t j = i * i; j < limit; j += i)
            {
                sieve.set(j, true);
            }
        }
    }

    if (print)
    {
        for (uint64_t i = 2; i < limit; ++i)
        {
            if (!sieve.get(i))
            {
                cout << i << "\n";
            }
        }
    }


    return 0;
}

