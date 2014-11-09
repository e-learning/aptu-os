#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>

using namespace std;

int n = 0, t = 0;           /* n = nums[] array size, t = number of threads, from command line */
vector<long int> numbers;
vector<int> done_sw;
vector<int> did_work_sw;

void *zero_multiples(void *threadid)
{
    int prime = 0;  /* current prime */
    int i_prime= 0; /* current prime index in nums[] */
    int i, k;       /* for looping */

    /* Each thread reads nums[] up to sqrt(n)
       If a positive number is encountered, it is prime:
         the number is negated, and all multiples are zeroed
         then continues looping looking for positive (prime) numbers */
    for(i = 0; i*i <= n; i++) /* read nums[] until locate a positive number or until sqrt(n) */
    {
        prime = numbers[i];
        i_prime = i;

        if(prime>0) /* if locate a positive number, it must be prime */
        {
            did_work_sw[(intptr_t) threadid]=1; /* indicates that this thread did some work */
            numbers[i_prime] = -numbers[i_prime]; /* set current prime to negative */
            for (k = i_prime + prime; k < n; k = k + prime) /* mark multiples to 0 */
            {
                numbers[k] = 0;
            }
        }
    }

    done_sw[(intptr_t) threadid]=1;  /* indicate that this thread is complete -- no more primes left */

    pthread_exit(NULL);
}

/* used for time elapsed */
/* Subtract the `struct timeval' values X and Y,
    storing the result in RESULT.
    Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
        tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

int main (int argc, char *argv[])
{
    pthread_t *threads = NULL;  /* threads structure */
    int rc;                     /* return code for pthread_create() */

    int c = 0;                  /* command line arguments */
    int i = 0, k = 0;           /* for loops */
    struct timeval tv_1;        /* time before */
    struct timeval tv_2;        /* time after */
    struct timeval result;      /* time result */

    int debug_level = 0;        /* used for verbose messaging to screen */
    int done_sw_main = 0;       /* used to check if all the threads have finished */
    int did_work_howmany = 0;   /* tallies how many threads actually did work */

    int n_primes = 0;           /* tallies the number of primes found */

    /*** Parse command line arguments *****************/
    while( (c = getopt( argc, argv,  "n:t:v" )) != EOF )
    {
        switch( c )
        {
            case 'n': n = strtol( optarg, (char **)NULL, 10 );
            break;

            case 't': t = strtol( optarg, (char **)NULL, 10 );
            break;

            case 'v': debug_level++;
            break;
        }
    }

    //if(debug_level) printf("n=%d, t=%d\n", n, t);   /* print n and t */

    numbers.resize(n);

    /* population nums[] array from 1 to n */
    for(i=1; i<n; i++) {numbers[i]=i+1;}   /* start at index 1 so that number 1 starts zeroed out */

    threads = (pthread_t *)malloc(t * sizeof(pthread_t));   /* allocate threads[] structure array */
    if(threads == NULL){fprintf(stderr, "threads Out of memory!\n");exit( EXIT_FAILURE );}

    //done_sw = (int *)malloc(t * sizeof(int));               /* allocate done_sw[] array */
    //if(done_sw == NULL){fprintf(stderr, "done_sw Out of memory!\n");exit( EXIT_FAILURE );}
    done_sw.resize(t);

    //did_work_sw = (int *)malloc(t * sizeof(int));           /* allocate did_work_sw[] array */
    did_work_sw.resize(t);
    //if(did_work_sw == NULL){fprintf(stderr, "did_work_sw Out of memory!\n");exit( EXIT_FAILURE );}

    /* create threads and run zero_multiples */
    for(i=0; i<t; i++){
        if(debug_level>1) printf("Creating thread %d\n", i);
        rc = pthread_create(&threads[i], NULL, zero_multiples,
        		(void *)(intptr_t)i);
        if (rc){printf("ERROR; return code from pthread_create() is %d\n", rc);exit(-1);}
    }

    /* main program wait until all threads are complete */
    while(done_sw_main < t) /* exit only when all threads have set their done_sw */
    {
        done_sw_main = 0;
        for(i=0; i<t; i++)
        {
            done_sw_main = done_sw_main + done_sw[i];   /* count how many threads are done */
        }
    }

    /* count number of threads that did work */
    did_work_howmany = 0;
    for(i=0; i<t; i++){did_work_howmany = did_work_howmany + did_work_sw[i];}

    /* count the number of primes found */
    if(debug_level)
    {
        n_primes = 0;
        for(k=0; k < n; k++)
        {
            if(numbers[k] != 0){n_primes++;} /* primes are all the non 0 numbers remaining in nums[] */
        }
        printf("n_primes=%d\n", n_primes);
    }

    timeval_subtract(&result,&tv_2,&tv_1);  /* calculate elapsed time */

    /* report results */
    //printf("%d %d %d %d %d.%06d\n", n, t, did_work_howmany, n_primes, result.tv_sec, result.tv_usec);

    pthread_exit(NULL); /* all done */
}
