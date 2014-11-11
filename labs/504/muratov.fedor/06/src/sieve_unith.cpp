#include <iostream>
#include <vector>
#include <stdlib.h>
#include <atomic>
#include <thread>
#include <mutex>
using namespace std;

typedef long long llong;

bool print = false;
llong N = 0;
llong currentNum = 2;
vector<char> arr;


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
		if (argc == 2)
		{
			N = atoll(argv[1]);
		}
		else
		{
			print = true;
			N = atoll(argv[2]);
		}
	}
	arr = vector<char>(N, 1);

	calculateOneThreadSieve();

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
		llong currentMultiplayer = currentNum++;
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




