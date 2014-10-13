#include <string.h>
#include <stdlib.h>
#include "requests.h"
struct Request parseRequest(const char *requestLine)
{
    struct Request result;
    if(strncmp(requestLine, "ALLOC ", 6)==0)
    {
        result.type = ALLOC;
        result.parameter = atoi(requestLine+6);
    }
    else if(strncmp(requestLine, "FREE ", 5)==0)
    {
        result.type = FREE;
        result.parameter = atoi(requestLine+5);
    }
    else if(strncmp(requestLine, "INFO", 4)==0)
    {
        result.type = INFO;
    }
    else if(strncmp(requestLine, "MAP", 3)==0)
    {
        result.type = MAP;
    }
    else result.type = UNKNOWN;
    return result;
};