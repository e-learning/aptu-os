#include <vector>
#include <iostream>
#include <numeric>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

using namespace std;

vector<long int> numbers;
vector<int> thread_is_done;
vector<int> thread_did_work;

void *zero_multiples(void *threadid) {
	int prime = 0;
	int prime_ind = 0;

	for (size_t i = 0; i * i <= numbers.size(); i++) {
		prime = numbers[i];
		prime_ind = i;

		if (prime > 0)
		{
			thread_did_work[(intptr_t) threadid] = 1;
			numbers[prime_ind] = -numbers[prime_ind];
			for (size_t k = prime_ind + prime; k < numbers.size(); k = k + prime)
			{
				numbers[k] = 0;
			}
		}
	}

	thread_is_done[(intptr_t) threadid] = 1;

	pthread_exit(NULL);
}

void print_usage() {
    cout << "usage:" << endl;
    cout << "./sieve_unith -p N" << endl;
    cout << "where:" << endl;
    cout << " N <-- sieve limit" << endl;
    cout << "-p <-- print finded primes, optional parameter" << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 1) {
		print_usage();
		return 0;
	}

	numbers.resize(atoi(argv[1]));

	for (size_t i = 1; i < numbers.size(); i++) {
		numbers[i] = i + 1;
	}


	if (argc >= 4 && string(argv[3]) == "-p") {
		for (size_t k = 0; k < numbers.size(); k++) {
			if (numbers[k] != 0) {
				cout << abs(numbers[k]) << endl;
			}
		}
	}

	pthread_exit(NULL);
}
