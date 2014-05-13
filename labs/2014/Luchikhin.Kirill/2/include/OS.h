#ifndef OS_H
#define OS_H

#include "Process.h"
#include "Scheduler.h"

#include <cstddef>

struct OS {
    OS(size_t c) : c(c)
    {}

    void addProcess(process_ptr p);
    void run();

private:
    deque<process_ptr> processes;
    size_t c;
};

#endif // OS_H
