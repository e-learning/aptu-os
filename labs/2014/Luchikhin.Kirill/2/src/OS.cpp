#include "../include/OS.h"
#include "../include/Process.h"

void OS::addProcess(process_ptr p)
{
    processes.push_back(p);
}

void OS::run()
{
    Scheduler::startScheduler(processes, c);
}
