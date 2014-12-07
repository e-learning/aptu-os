#include "get_params.hpp"
#include "texceptions.hpp"
#include <cerrno>



Params get_params(int argc, char* argv[], bool is_multithreaded)
{    
    Params params;
    int n_arg_ind, threads_num_arg_ind;
    
    
    if (argc == 3 + static_cast<int>(is_multithreaded))
    {
        if (argv[1][0] != '-')
        {
            throw TException(TException::Errors::USAGE);
        }
        if (strcmp(&argv[1][1], "p") == 0)
        {
            params.is_print = true; 
        }
        else
        {
            throw TException(TException::Errors::USAGE);
        }
        n_arg_ind = 2;
        threads_num_arg_ind = n_arg_ind + 1;
    }
    else if (argc == 2 + static_cast<int>(is_multithreaded))
    {
        n_arg_ind = 1;
        threads_num_arg_ind = n_arg_ind + 1;
        params.is_print = false; 
    } 
    else
    {
        throw TException(TException::Errors::USAGE);
    }
    
    char* end;
    params.N = static_cast<uint64_t>(strtoumax(argv[n_arg_ind], &end, 10));
    if (errno == ERANGE || end == argv[n_arg_ind] || *end != '\0')
    {
        throw TException(TException::Errors::INVALID_NUMBER);
    }
    
    if (is_multithreaded)
    {
        params.threads_num = static_cast<uint32_t>(strtoumax(argv[threads_num_arg_ind], &end, 10));
        if (params.threads_num == 0 || errno == ERANGE || end == argv[threads_num_arg_ind] || *end != '\0')
        {
            throw TException(TException::Errors::INVALID_THREADS_NUMBER);
        }
    }
    
    return params;
}
