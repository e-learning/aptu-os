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

void mark_primes() {
	int prime = 0;
	int prime_ind = 0;

	for (size_t i = 0; i * i <= numbers.size(); i++) {
		prime = numbers[i];
		prime_ind = i;

		if (prime > 0) {
			numbers[prime_ind] = -numbers[prime_ind];
			for (size_t k = prime_ind + prime; k < numbers.size(); k = k + prime) {
				numbers[k] = 0;
			}
		}
	}
}

void print_usage() {
	cout << "usage:" << endl;
	cout << "./sieve_unith -p N" << endl;
	cout << "where:" << endl;
	cout << "-p <-- print finded primes, optional parameter" << endl;
	cout << " N <-- sieve limit" << endl;
}

int main(int argc, char *argv[]) {
	clock_t start, end;
	start = clock();

	if (argc < 2) {
		print_usage();
		return 0;
	}

	if (string(argv[1]) == "-p") {
		numbers.resize(atoi(argv[2]));
	} else {
		numbers.resize(atoi(argv[1]));
	}

	for (size_t i = 1; i < numbers.size(); i++) {
		numbers[i] = i + 1;
	}

	mark_primes();

	if (string(argv[1]) == "-p") {
		for (size_t k = 0; k < numbers.size(); k++) {
			if (numbers[k] != 0) {
				cout << abs(numbers[k]) << endl;
			}
		}
	}

	end = clock();
	float exec_time((float) end - (float) start);
	if (string(argv[1]) != "-p") {
		cout << exec_time << endl;
	}

	return 0;
}
