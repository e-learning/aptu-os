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

int m;
char* primes;
unsigned long long next_prime;
unsigned long long n;
unsigned long long sqrt_n;
pthread_mutex_t next_prime_lock;

void* thread_function(void* arg)
{
	unsigned long long i;
	unsigned long long j;
	while (1)
	{
		pthread_mutex_lock(&next_prime_lock);
		while (!primes[next_prime] && next_prime <= sqrt_n) next_prime++;
		if (next_prime > sqrt_n)
		{
			pthread_mutex_unlock(&next_prime_lock);
			return NULL;
		}
		i = next_prime;
		next_prime++;
		pthread_mutex_unlock(&next_prime_lock);
		for (j = i * i; j < n; j += i)
		{
			primes[j] = 0;
		}
	}
}


pthread_t* start_threads(int count)
{
	int i;
	pthread_t* threads = (pthread_t*)malloc(count * sizeof(pthread_t));
	for (i = 0; i < count; i++)
	{
		pthread_create(threads + i, NULL, thread_function, NULL);
	}
	return threads;
}


void join_threads(int count, pthread_t* threads)
{
	int i;
	for (i = 0; i < count; i++)
	{
		pthread_join(threads[i], NULL);
	}
	free(threads);
}


char* eratosthenes_sieve()
{
	sqrt_n = (unsigned long long)sqrt((double)n);
	primes = (char*)malloc(n + 1);
	memset(primes, 0x01, n + 1);
	primes[0] = 0;
	primes[1] = 0;
	next_prime = 2;
	pthread_mutex_init(&next_prime_lock, NULL);
	pthread_t* threads = start_threads(m);
	join_threads(m, threads);
	pthread_mutex_destroy(&next_prime_lock);
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