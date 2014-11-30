#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <ctime>

using namespace std;

void threadFun(int beg, int s, vector<char> &prime)
{
    for (unsigned int i = beg + 2; i < prime.size(); i += s)
        if (prime[i])
            if (i * 1ll * i <= prime.size() - 1)
                for (unsigned int j=i*i; j<prime.size(); j+=i)
                    prime[j] = false;
}

int main(int argc, char *argv[])
{
    clock_t start;
    start = clock();
    int n;
    int m;
    if (argc == 3)
    {
        n = atoi(argv[1]);
        m = atoi(argv[2]);
    }
    else
    {
        n = atoi(argv[2]);
        m = atoi(argv[3]);
    }
    vector<char> prime (n+1, true);
    prime[0] = prime[1] = false;
    thread *threads = new thread[n];
    for (int i = 0; i < n - 1; i++)
    {
        threads[i] = thread(threadFun, i, m, ref(prime));
        threads[i].detach();
    }
    threads[n-1] = thread(threadFun, n-1, m, ref(prime));
    threads[n-1].join();
    if (argc > 1 && strcmp(argv[1], "-p") == 0)
    {
        for (int i = 2; i <=n; i++)
            if (prime[i])
                cout << i << " ";
    }
    cout << endl;
    cout << clock() - start;
    delete[] threads;
    return 0;
}

