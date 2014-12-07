#include "texceptions.hpp"
#include <cstring>
#include <iostream>


TException::TException() { }

TException::TException(const Errors err_code)
{ 
    switch (err_code)
    {
        case USAGE:
            if (strcmp(__FILE__, "sieve_unith") == 0)
            {
                msg = "Usage: sieve_unith [-p] NUMBER";
            }
            else
            {
                msg = "Usage: sieve_multith [-p] NUMBER THREADS_NUMBER";
            }
            break;
        case INVALID_NUMBER:
            msg = "Error: invalid number (not a number or too large number (out of 64bit))";
            break;
        case INVALID_THREADS_NUMBER:
            msg = "Error: invalid threads number (zero, not a number or too large number (out of 32bit))";
            break;
        case THREAD_CREATE_FAILED:
            msg = "Error: cannot create thread";
            break;
        default:
            msg = "Unknown error";
            break;
    }
}

TException::~TException() throw() { };

const char* TException::what() const throw()
{
    return msg.c_str();
}
