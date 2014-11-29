#include <iostream>
#include <vector>
#include <cstring>
#include <thread>

using namespace std;

int main(int argc, char *argv[])
{
    clock_t start;
    start = clock();
    int n;
    if (argc == 2)
    {
        n = atoi(argv[1]);
    }
    else
    {
        n = atoi(argv[2]);
    }
    vector<char> prime (n+1, true);
    prime[0] = prime[1] = false;
    for (int i=2; i<=n; ++i)
        if (prime[i])
            if (i * 1ll * i <= n)
                for (int j=i*i; j<=n; j+=i)
                    prime[j] = false;
    if (argc > 1 && strcmp(argv[1], "-p") == 0)
    {
        for (int i = 2; i <=n; i++)
            if (prime[i])
                cout << i << " ";
    }
    cout << endl;
    cout << clock() - start;
    return 0;
}

