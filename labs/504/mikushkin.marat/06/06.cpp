#include <vector>
#include <iostream>
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

int main(int argc, char *argv[]) {
	vector<pthread_t> threads;

	numbers.resize(atoi(argv[1]));
	threads.resize(atoi(argv[2]));

	for (size_t i = 1; i < numbers.size(); i++) {
		numbers[i] = i + 1;
	} /* start at index 1 so that number 1 starts zeroed out */


	thread_is_done.resize(threads.size());
	thread_did_work.resize(threads.size());

	for (size_t i = 0; i < threads.size(); i++) {
		pthread_create(&threads[i], NULL, zero_multiples, (void *) (intptr_t) i);
	}

	/* main program wait until all threads are complete */
	size_t is_all_thread_done = 0;
	while (is_all_thread_done < threads.size()) /* exit only when all threads have set their done_sw */
	{
		is_all_thread_done = 0;
		for (size_t i = 0; i < threads.size(); i++) {
			is_all_thread_done = is_all_thread_done + thread_is_done[i]; /* count how many threads are done */
		}
	}

	if (true) {
		for (size_t k = 0; k < numbers.size(); k++) {
			if (numbers[k] != 0) {
				cout << abs(numbers[k]) << endl;
			}
		}
	}

	pthread_exit(NULL);
}
