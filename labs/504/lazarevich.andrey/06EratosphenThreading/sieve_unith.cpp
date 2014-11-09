#include <iostream>
#include <cstdint>
#include <limits>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

vector<bool> S;
u_int64_t n;
bool debug = false;

void parse_cmd(int argc, char **argv)
{
    const string debug_flag = "-p";
    switch (argc)
    {
        case 1:
            cout << "Usage: " << argv[0] << " <debug_flag> <limit number>" << endl;
            exit(1);
            break;
        case 2:
            if (argv[1] == debug_flag)
            {
                cout << "No limit number. Run again." << endl;
                exit(1);
            }
            n = stoi(argv[1]);
            if (n <= 2)
            {
                cout << "Not a correct number" << endl;
                exit(1);
            }
            break;
        case 3:
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
            break;
        default:
            cout << "Wrong number of input parameters" << endl;
    }
}

void sieve()
{
    for (u_int64_t i = 2; i * i < n; ++i)
    {
        if (S[i])
        {
            for (u_int64_t j = i * i; j < n; j += i)
            {
                S[j] = false;
            }
        }
    }

}

int main(int argc, char **argv)
{
    parse_cmd(argc, argv);
    S = vector<bool>(n);
    S[0] = false;
    S[1] = false;
    fill(S.begin() + 2, S.end(), true);
    chrono::high_resolution_clock clock;

    chrono::time_point<chrono::high_resolution_clock> start = clock.now();
    sieve();
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

