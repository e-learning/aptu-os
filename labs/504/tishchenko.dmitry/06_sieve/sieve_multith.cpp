#include <iostream>
#include <string.h>
#include <atomic>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define PROGRAMMNAME "sieve_multith"

using namespace std;
struct thread_context
{
    int upper_bound;
    atomic<int> cur_global;
    bool *isComposite;
};
void* eratosthenesSieveThread(void *cntxt)
{
    thread_context *context = (thread_context*) cntxt;
    int upper_bound = context->upper_bound;
    int cur_local;
    do {
        cur_local = context->cur_global.fetch_add(1);
        if (!(context->isComposite[cur_local])) {
            for (int k = cur_local * cur_local; k < upper_bound; k += cur_local) {
                context->isComposite[k] = true;
            }
        }
    } while (cur_local * cur_local < upper_bound);
    pthread_exit(nullptr);
}
void runEratosthenesSieveThreads(int upperBound, int threads_count, bool *isComposite) {
    pthread_t *threads = new pthread_t[threads_count];
    thread_context global_context;
    global_context.upper_bound = upperBound;
    global_context.cur_global.store(2);
    global_context.isComposite = isComposite;
    for (int i = 0; i < threads_count; ++i) {
        pthread_create(&threads[i], 0, eratosthenesSieveThread, &global_context);
    }
    for (int i = 0; i < threads_count; ++i) {
        pthread_join(threads[i], 0);
    }
    delete[] threads;
}

void print_using()
{
    cout << "Using: " <<PROGRAMMNAME <<"[-p] N M" << endl << \
            "-p: print out found numbers" << endl << \
            "N: upper bound for search interval" << endl << \
            "M: count of threads to run" << endl;
}

bool parse_args(int argc, char **argv, int &upper_bound, int &threads_number, bool &print_needed) {
    //format: programmname [-p] N M
    char *Nptr, *Nendptr, *Mptr, *Mendptr;
    if (argc == 3) //without print
    {
        print_needed = false;
        Nptr = argv[1];
        Mptr = argv[2];
    }
    else if (argc == 4) //with print
    {
        if(strcmp(argv[1], "-p")!=0)
        {
            return false; //incorrect arg instead of -p
        }
        print_needed = true;
        Nptr = argv[2];
        Mptr = argv[3];
    }
    else {
        return false; //not enough or too many args
    }
    Nendptr = Nptr;
    Mendptr = Mptr;
    upper_bound = strtol(Nptr, &Nendptr, 10);
    if ((Nendptr == Nptr) || (*Nendptr != '\0')) {
        return false; //N's not integer
    }
    threads_number = strtol(Mptr, &Mendptr, 10);
    if ((Nendptr == Mptr) || (*Mendptr != '\0')) {
        return false; //M's not integer
    }
    return true;
}

int main(int argc, char **argv) {
    int upper_bound, threads_count;
    bool print_needed;
    if(!parse_args(argc, argv, upper_bound, threads_count, print_needed))
    {
        print_using();
        return EXIT_FAILURE;
    }
    bool *isComposite = (bool*)calloc(upper_bound, sizeof(bool));
    runEratosthenesSieveThreads(upper_bound, threads_count, isComposite);
    if(print_needed)
    {
        for (int number = 2; number < upper_bound; number++)
        {
            if (!isComposite[number])
            {
                cout << number << endl;
            }
        }
    }
    free(isComposite);
    return 0;
}
