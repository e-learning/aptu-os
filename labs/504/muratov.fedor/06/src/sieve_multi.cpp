#include <iostream>
#include <vector>
#include <stdlib.h>
#include <atomic>
#include <thread>
#include <mutex>
using namespace std;

bool print = false;
unsigned long long N = 0;
int M = 0;
atomic<int> threadQuantity;
vector<bool> arr;
mutex mtx;

typedef long long llong;
void calculateMultiSieve();
void deleteMultiples(llong multiplayer);
void calculateSingleSieve(vector<bool> &arr);

int main(int argc, char* argv[])
{
	threadQuantity = 0;
	if (argc == 1)
	{
		cout << "Single thread version of Sieve of Eratosthenes" << endl;
		cout << "First parameter -p (optional). If specifed, than print sieve" << endl;
		cout << "Parameter N - quantity of sieve"<<endl;
		getchar();
		return 0;
	}
	else
	{
		if (argc == 3)
		{
			N = atoll(argv[1]);
			M = atoi(argv[2]);
		}
		else
		{
			print = true;
			N = atoll(argv[2]);
			M = atoi(argv[3]);
		}
	}
	arr = vector<bool>(N, true);
	calculateMultiSieve();
	if (print)
	{
		for (int i = 0; i < arr.size(); ++i)
		{
			if (arr[i])
				cout << i << " ";
		}
		getchar();	
	}

	return 0;
}

void calculateMultiSieve()
{
	
	arr[0] = arr[1] = false;
	for (llong i = 2; i*i < N; ++i)
	{
		if (arr[i])
		{
			while (threadQuantity > M);
			thread newThread = thread(deleteMultiples, i);
			newThread.detach();
		}
	}
	while (threadQuantity != 0);
}


void deleteMultiples(llong multiplayer)
{
	++threadQuantity;
	for (llong j = multiplayer*multiplayer; j < N; j += multiplayer)
	{
		mtx.lock();
		arr[j] = false;
		mtx.unlock();
	}
	--threadQuantity;
	
}

void calculateSingleSieve(vector<bool> &arr)
{
	arr[0] = arr[1] = false;
	for (llong i = 2; i*i < N; ++i)
	if (arr[i])
	for (llong j = i*i; j < N; j += i)
		arr[j] = false;
}