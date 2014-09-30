#include <string.h>
#include <stdlib.h>
#include "Process.h"

Process* ProcessCreate(char *name, int begin, int duration)
{
    Process *process = malloc(sizeof(Process));
    process->name = malloc(strlen(name) * sizeof(char));
    strcpy(process->name, name);
    process->begin = begin;
    process->duration = duration;
    process->locks = VectorCreate(sizeof(Lock));
    process->next_lock = 0;
    return process;
}

void ProcessExecute(Process *process, int duration)
{
    int duration_to_lock = ProcessGetDurationToLock(process);
    if (duration_to_lock < duration)
        duration = duration_to_lock;

    process->execution_time += duration;
}


int ProcessGetDurationToLock(Process *process)
{
    Lock* next_lock = VectorGet(process->locks, process->next_lock);
    if (!next_lock)
        return process->duration - process->execution_time;
    else
        return next_lock->begin - process->execution_time;
}

int ProcessGetDurationToReady(Process *process)
{
    Lock* next_lock = VectorGet(process->locks, process->next_lock);
    process->execution_time += ((Lock*)VectorGet(process->locks, process->next_lock))->duration;
    process->next_lock++;
    if (!next_lock)
        return -1;
    else
        return next_lock->duration;
}
