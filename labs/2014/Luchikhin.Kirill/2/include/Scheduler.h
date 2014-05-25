#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Process.h"
#include "IO.h"

#include <cstddef>

struct Scheduler
{
    void static startScheduler(deque<process_ptr>& processes, size_t c);
};

#endif // SCHEDULER_H
