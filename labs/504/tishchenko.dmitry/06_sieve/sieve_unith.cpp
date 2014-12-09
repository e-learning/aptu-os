#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define PROGRAMMNAME "sieve_unith"

using namespace std;

void runEratosthenesSieve(int upperBound, bool *isComposite) {
    int upperBoundSquareRoot = (int)sqrt((double)upperBound);
    for (int m = 2; m <= upperBoundSquareRoot; m++) {
        if (!isComposite[m]) {
            for (int k = m * m; k < upperBound; k += m)
                isComposite[k] = true;
        }
    }
}

void print_using()
{
    cout << "Using: " <<PROGRAMMNAME <<"[-p] number" << endl << \
            "-p: print out found numbers" << endl << \
            "number: upper bound for search interval" << endl;
}

bool parse_args(int argc, char **argv, int &upper_bound, bool &print_needed) {
    //format: programmname [-p] number
    char *nptr, *endptr;
    if (argc == 2) //without print
    {
        print_needed = false;
        nptr = argv[1];
    }
    else if (argc == 3) //with print
    {
        if(strcmp(argv[1], "-p")!=0)
        {
            return false; //incorrect arg instead of -p
        }
        print_needed = true;
        nptr = argv[2];
    }
    else {
        return false; //not enough or too many args
    }
    endptr = nptr;
    upper_bound = strtol(nptr, &endptr, 10);
    if ((endptr == nptr) || (*endptr != '\0')) {
        return false; //not integer
    }
    return true;
}

int main(int argc, char **argv) {
    int upper_bound;
    bool print_needed;
    if(!parse_args(argc, argv, upper_bound, print_needed))
    {
        print_using();
        return EXIT_FAILURE;
    }
    bool *isComposite = (bool*)calloc(upper_bound, sizeof(bool));
    runEratosthenesSieve(upper_bound, isComposite);
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
