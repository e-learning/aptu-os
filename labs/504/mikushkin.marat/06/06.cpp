#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <vector>

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
	int rc;

	size_t is_all_thread_done = 0;
	int number_of_active_threads = 0;

	size_t number_of_threads = atoi(argv[2]);
	numbers.resize(atoi(argv[1]));

	for (size_t i = 1; i < numbers.size(); i++) {
		numbers[i] = i + 1;
	} /* start at index 1 so that number 1 starts zeroed out */

	threads.resize(number_of_threads);

	thread_is_done.resize(number_of_threads);
	thread_did_work.resize(number_of_threads);

	/* create threads and run zero_multiples */
	for (size_t i = 0; i < number_of_threads; i++) {
		//if(debug_level>1) printf("Creating thread %d\n", i);
		rc = pthread_create(&threads[i], NULL, zero_multiples, (void *) (intptr_t) i); /* create thread to run zero_multiples() */
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	/* main program wait until all threads are complete */
	while (is_all_thread_done < number_of_threads) /* exit only when all threads have set their done_sw */
	{
		is_all_thread_done = 0;
		for (size_t i = 0; i < number_of_threads; i++) {
			is_all_thread_done = is_all_thread_done + thread_is_done[i]; /* count how many threads are done */
		}
	}

	/* count number of threads that did work */
	number_of_active_threads = 0;
	for (size_t i = 0; i < number_of_threads; i++) {
		number_of_active_threads = number_of_active_threads + thread_did_work[i];
	}

	if (true) {
		for (size_t k = 0; k < numbers.size(); k++) {
			if (numbers[k] != 0) {
				printf("%d\n", abs(numbers[k]));
			}
		}
	}

	pthread_exit(NULL);
}
