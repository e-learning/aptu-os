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

		if (prime > 0) {
			thread_did_work[(intptr_t) threadid] = 1;
			numbers[prime_ind] = -numbers[prime_ind];
			for (size_t k = prime_ind + prime; k < numbers.size(); k = k + prime) {
				numbers[k] = 0;
			}
		}
	}

	thread_is_done[(intptr_t) threadid] = 1;

	pthread_exit(NULL);
}

void print_usage() {
	cout << "usage:" << endl;
	cout << "./sieve_multith -p N M" << endl;
	cout << "where:" << endl;
	cout << "-p <-- print finded primes, optional parameter" << endl;
	cout << " N <-- sieve limit" << endl;
	cout << " M <-- number of threads" << endl;
}

int main(int argc, char *argv[]) {
	clock_t start, end;
	start = clock();

	if (argc < 3) {
		print_usage();
		return 0;
	}

	vector<pthread_t> threads;

	if (string(argv[1]) == "-p") {
		numbers.resize(atoi(argv[2]));
		threads.resize(atoi(argv[3]));
	} else {
		numbers.resize(atoi(argv[1]));
		threads.resize(atoi(argv[2]));
	}

	for (size_t i = 1; i < numbers.size(); i++) {
		numbers[i] = i + 1;
	}

	thread_is_done.resize(threads.size());
	thread_did_work.resize(threads.size());

	for (size_t i = 0; i < threads.size(); i++) {
		pthread_create(&threads[i], NULL, zero_multiples, (void *) (intptr_t) i);
	}

	size_t is_all_thread_done = 0;
	while (is_all_thread_done < threads.size()) {
		is_all_thread_done = accumulate(thread_is_done.begin(), thread_is_done.end(), 0);
	}

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

	pthread_exit(NULL);
}
