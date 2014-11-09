#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include<vector>

using namespace std;

vector<long int> numbers;
vector<int> done_sw;
vector<int> did_work_sw;

/* Function that each thread executes to zero out multiples of primes they find */
void *zero_multiples(void *threadid)
{
    int prime = 0;  /* current prime */
    int i_prime= 0; /* current prime index in nums[] */
    //int i, k;       /* for looping */

    /* Each thread reads nums[] up to sqrt(n)
       If a positive number is encountered, it is prime:
         the number is negated, and all multiples are zeroed
         then continues looping looking for positive (prime) numbers */
    for(size_t i = 0; i*i <= numbers.size(); i++) /* read nums[] until locate a positive number or until sqrt(n) */
    {
        prime = numbers[i];
        i_prime = i;

        if(prime>0) /* if locate a positive number, it must be prime */
        {
            did_work_sw[(intptr_t) threadid]=1; /* indicates that this thread did some work */
            numbers[i_prime] = -numbers[i_prime]; /* set current prime to negative */
            for (size_t k = i_prime + prime; k < numbers.size(); k = k + prime) /* mark multiples to 0 */
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

    struct timeval tv_1;        /* time before */
    struct timeval tv_2;        /* time after */
    struct timeval result;      /* time result */

    size_t done_sw_main = 0;       /* used to check if all the threads have finished */
    int did_work_howmany = 0;   /* tallies how many threads actually did work */

    size_t number_of_threads = atoi(argv[2]);
    gettimeofday(&tv_1, 0);
    numbers.resize(atoi(argv[1]));

    /* population nums[] array from 1 to n */
    for(size_t i=1; i<numbers.size(); i++) {numbers[i]=i+1;}   /* start at index 1 so that number 1 starts zeroed out */

    threads = (pthread_t *)malloc(number_of_threads * sizeof(pthread_t));   /* allocate threads[] structure array */
    if(threads == NULL){fprintf(stderr, "threads Out of memory!\n");exit( EXIT_FAILURE );}

    done_sw.resize(number_of_threads);
    did_work_sw.resize(number_of_threads);

    /* create threads and run zero_multiples */
    for(size_t i=0; i<number_of_threads; i++){
        //if(debug_level>1) printf("Creating thread %d\n", i);
        rc = pthread_create(&threads[i], NULL, zero_multiples, (void *)(intptr_t)i); /* create thread to run zero_multiples() */
        if (rc){printf("ERROR; return code from pthread_create() is %d\n", rc);exit(-1);}
    }

    /* main program wait until all threads are complete */
    while(done_sw_main < number_of_threads) /* exit only when all threads have set their done_sw */
    {
        done_sw_main = 0;
        for(size_t i=0; i<number_of_threads; i++)
        {
            done_sw_main = done_sw_main + done_sw[i];   /* count how many threads are done */
        }
    }

    /* count number of threads that did work */
    did_work_howmany = 0;
    for(size_t i=0; i<number_of_threads; i++){did_work_howmany = did_work_howmany + did_work_sw[i];}

    if(true)
    {
        for(size_t k=0; k < numbers.size(); k++)
        {
            if(numbers[k] != 0)
            {
            	printf("%d\n", abs(numbers[k]));
            }
        }
    }

    gettimeofday(&tv_2,0);           /* stop timer */
    timeval_subtract(&result,&tv_2,&tv_1);  /* calculate elapsed time */

    pthread_exit(NULL); /* all done */
}
