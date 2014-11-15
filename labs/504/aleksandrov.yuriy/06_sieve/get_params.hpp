#pragma once

#include <cinttypes>
#include <cstring>


struct Params
{
    bool is_print;
    uint64_t N;
    uint32_t threads_num;
};

Params get_params(int argc, char* argv[], bool is_multithreaded);
