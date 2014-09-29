#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

struct IO
{
    int start_io;
    int end_io;
};

enum Status
{
    ReadyH = 0, //High Prio
    ReadyL = 1, //Low Prio
    Runned = 2,
    Blocked = 3
};

struct Task
{
    std::vector<IO>     io_tasks;
    std::string         name;
    int                 start_time;
    int                 end_time;
    Status              status;
    int                 quant_avail;
};

int cur_time;

enum CPU
{
    Idle = 0,
    Work = 1
};

#endif // COMMON_HPP
