#pragma once

#include <exception>
#include <string>

using std::exception;
using std::string;


class TException : public exception
{
private:
    string msg;
public:
    enum Errors
    {
        USAGE,
        INVALID_NUMBER,
        INVALID_THREADS_NUMBER,
        THREAD_CREATE_FAILED
    };
    
    TException();
    
    TException(const Errors err_code);
    
    ~TException() throw();
    
    const char* what() const throw();
};
