#include <iostream>
#include <vector>
#include <stdlib.h>
#include <atomic>
#include <thread>
#include <mutex>
using namespace std;

typedef long long llong;

bool print = false;
unsigned long long N = 0;
int M = 0;
atomic <llong> currentNum;
vector<char> arr;
mutex mtx;


void calculateOneThreadSieve();

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "Single thread version of Sieve of Eratosthenes" << endl;
		cout << "First parameter -p (optional). If specifed, than print sieve" << endl;
		cout << "Parameter N - quantity of sieve";
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
	currentNum = 2;
	arr = vector<char>(N, 1);
	vector<thread> threads(M);
	for (int th_num = 0; th_num < M; ++th_num)
	{
		threads[th_num] = thread(calculateOneThreadSieve);
	}
	for (int th_num = 0; th_num < M; ++th_num)
	{
		threads[th_num].join();
	}

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

void calculateOneThreadSieve()
{	
	while (true)
	{
		llong currentMultiplayer = currentNum.fetch_add(1);
		if (currentMultiplayer * currentMultiplayer >= N)
		{
			return;
		}
		if (arr[currentMultiplayer])
		{
			for (llong currentValue = currentMultiplayer * currentMultiplayer; currentValue < N; currentValue += currentMultiplayer)
				arr[currentValue] = 0;
		}
	}
}




