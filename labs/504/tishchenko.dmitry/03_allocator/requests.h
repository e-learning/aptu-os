#pragma once
enum RequestType
{
    ALLOC,
    FREE,
    INFO,
    MAP,
    UNKNOWN
};
struct Request
{
    enum RequestType type;
    unsigned short parameter;
};

struct Request parseRequest(const char *requestLine);