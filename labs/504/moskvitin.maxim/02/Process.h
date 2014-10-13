#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "Vector.h"

typedef struct{
    int begin;
    int duration;
} Lock;

typedef struct{
    char *name;
    int duration;
    int begin;
    int execution_time;
    int next_lock;
    Vector* locks;
} Process;


Process* ProcessCreate(char *name, int begin, int duration);
void ProcessExecute(Process *process, int duration);
int ProcessGetDurationToLock(Process *process);
int ProcessGetDurationToReady(Process *process);

#endif