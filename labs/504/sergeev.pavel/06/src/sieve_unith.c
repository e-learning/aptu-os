#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


struct Settings
{
	int output_result; // if set write result to console
	unsigned long long N; // max value for sieve
};


struct Settings read_settings(int argc, char** argv)
{
	struct Settings settings;
	if (argc < 2)
	{
		printf("wrong arguments count\n");
		exit(-1);
	}
	if (strcmp(argv[1], "-p") == 0)
	{
		settings.output_result = 1;
		settings.N = strtoll(argv[2], NULL, 10);
	}
	else
	{
		settings.output_result = 0;
		settings.N = strtoll(argv[1], NULL, 10);
	}
	return settings;
}

 
char* eratosthenes_sieve(unsigned long long n)
{
	char* primes;
	unsigned long long i, j, sqrt_n;

	sqrt_n = (unsigned long long)sqrt((double)n);
	primes = (char*)malloc(n + 1);
	memset(primes, 0x01, n + 1);
	primes[0] = 0;
	primes[1] = 0;
	for(i = 2; i <= sqrt_n; i++)
	{
		if (primes[i])
		{
			for (j = i * i; j < n; j += i)
			{
				primes[j] = 0;
			}
		}
	}
  	return primes;
}


int main(int argc, char** argv)
{
	unsigned long long i;
	struct Settings settings = read_settings(argc, argv);

	char* primes = eratosthenes_sieve(settings.N);

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