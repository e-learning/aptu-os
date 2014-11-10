
#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;

bool print = false;
unsigned long long N = 0;

vector<bool> arr;

void calculateSingleSieve(vector<bool> &arr);

int main(int argc, char* argv[])
{
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
	arr = vector<bool>(N, true);
	calculateSingleSieve(arr);
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

void calculateSingleSieve(vector<bool> &arr)
{
	arr[0] = arr[1] = false;
	for (int i = 2; i*i < arr.size(); ++i)
	if (arr[i])
	for (int j = i*i; j < arr.size(); j += i)
		arr[j] = false;
}

