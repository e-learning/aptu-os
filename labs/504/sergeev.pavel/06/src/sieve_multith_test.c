#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>


struct Settings
{
	int output_result; // if set write result to console
	unsigned long long N; // max value for sieve
	int M; // threads count
};


struct Settings read_settings(int argc, char** argv)
{
	struct Settings settings;
	if (argc < 3)
	{
		printf("wrong arguments count\n");
		exit(-1);
	}
	if (strcmp(argv[1], "-p") == 0)
	{
		settings.output_result = 1;
		settings.N = strtoll(argv[2], NULL, 10);
		settings.M = strtoll(argv[3], NULL, 10);
	}
	else
	{
		settings.output_result = 0;
		settings.N = strtoll(argv[1], NULL, 10);
		settings.M = strtoll(argv[2], NULL, 10);
	}
	return settings;
}


struct Task
{
	unsigned long long base;
	unsigned long long step;
	pthread_mutex_t task_seted_mutex;
	pthread_cond_t task_ready;
	pthread_mutex_t task_ready_mutex;
};

pthread_cond_t tasks_seted;
struct Task* tasks;
int* thread_num;
int m;
char* primes;
unsigned long long n;
unsigned long long sqrt_n;


void* thread_function(void* arg)
{
	// waiting tasks ready
	int thread_num = *arg;
	while (1)
	{
		pthread_mutex_lock(&(tasks[i].task_seted_mutex));
		pthread_cond_wait(&tasks_seted, &(tasks[i].task_seted_mutex));
	


		pthread_mutex_unlock(&(tasks[i].task_seted_mutex));
	}
	return NULL;
}


pthread_t* start_threads(int count)
{
	int i;
	pthread_t* threads = (pthread_t*)malloc(count * sizeof(pthread_t));
	tasks = (struct Task*)malloc(count * sizeof(struct Task));
	thread_num = (int*)malloc(count * sizeof(int));
	for (i = 0; i < count; i++)
	{
		pthread_mutex_init(&(tasks[i].task_seted_mutex), NULL);
		pthread_mutex_init(&(tasks[i].task_ready_mutex), NULL);
		pthread_cond_init(&(tasks_seted), NULL);
		// pthread_cond_init(&(tasks[i].task_seted), NULL);
		// pthread_cond_init(&(tasks[i].task_ready), NULL);
		//pthread_mutex_lock(&(tasks[i].task_ready_mutex));
		thread_num[i] = i;
		pthread_create(threads + i, &(thread_num[i]), thread_function, NULL);
	}
	return threads;
}


void join_threads(int count, pthread_t* threads)
{
	int i;
	for (i = 0; i < count; i++)
	{
		pthread_join(threads[i], NULL);
		pthread_mutex_destroy(&(tasks[i].task_seted_mutex));
		pthread_mutex_destroy(&(tasks[i].task_ready_mutex));
		pthread_cond_destroy(&(tasks_seted));
		// pthread_cond_destroy(&(tasks[i].task_seted));
		// pthread_cond_destroy(&(tasks[i].task_ready));
	}
	free(threads);
	free(tasks);
	free(thread_num);
}


void set_tasks()
{

}


void wait_tasks_completition()
{
	
}


char* eratosthenes_sieve()
{
	int i;
	sqrt_n = (unsigned long long)sqrt((double)n);
	primes = (char*)malloc(n + 1);
	memset(primes, 0x01, n + 1);
	primes[0] = 0;
	primes[1] = 0;
	pthread_t* threads = start_threads(m);

	for (i = 2; i <= sqrt_n; i++)
	{
		//waiting all threads
		if (primes[i])
		{
			set_tasks();
			pthread_cond_broad_cast(&tasks_seted);
			wait_tasks_completition();
		}
	}

	join_threads(m, threads);
  	return primes;
}


int main(int argc, char** argv)
{
	unsigned long long i;
	struct Settings settings = read_settings(argc, argv);
	n = settings.N;
	m = settings.M;
	char* primes = eratosthenes_sieve();

	if (settings.output_result)
	{
		for (i = 2; i < settings.N; i++)
		{
			if (primes[i])
			{
				printf("%llu\n", i);
			}
		}
	}
	free(primes);
	return 0;
}