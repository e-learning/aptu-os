#include "Heap.h"
#include "Vector.h"
#include "Process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int quantum;
    scanf("%d\n", &quantum);

    char *line = NULL;
    size_t line_size = 0;
    char *name = NULL;
    size_t name_size = 0;
    
    Vector *processes = VectorCreate(sizeof(Process));
    while(getdelim(&name, &name_size, ' ', stdin) != -1)
    {
        getline(&line, &line_size, stdin);
        int begin;
        int duration;

        int offset;
        char* p_line = line;
        sscanf(p_line, "%d %d%n", &begin, &duration, &offset);
        p_line += offset;
        Process *process = ProcessCreate(name, begin, duration);

        while (sscanf(p_line, "%d %d%n", &begin, &duration, &offset) == 2)
        {
            p_line += offset;
            Lock lock;
            lock.begin = begin;
            lock.duration = duration;
            VectorPush(process->locks, &lock);
        }
        VectorPush(processes, process);

        free(line);
        line_size = 0;
        free(name);
        name_size = 0;
    }

    Heap *ready = NULL;
    Heap *locked = NULL;
    size_t i;
    for (i = 0; i < processes->size; ++i)
    {
        Process *process = VectorGet(processes, i);
        locked = HeapInsert(locked, process->begin, i);
    }

    int time = 0;
    int current_process_id = -1;
    while (ready || locked || current_process_id != -1)
    {
        while (locked && locked->key <= time)
        {
            int idx = HeapPop(&locked);
            Process *process = VectorGet(processes, idx);
            ready = HeapInsert(ready, ProcessGetDurationToLock(process), idx);
        }

        int idx_prior = -1;
        if (ready)
            idx_prior = ready->id;

        Process *priorProcess = NULL;
        if (idx_prior != -1)
            priorProcess = VectorGet(processes, idx_prior);

        Process *curProcess = NULL;
        if (current_process_id != -1)
            curProcess = VectorGet(processes, current_process_id);

        if (priorProcess && (!curProcess ||
                    ProcessGetDurationToLock(curProcess) > ProcessGetDurationToLock(priorProcess)))
        {
            if (curProcess) {
                HeapInsert(ready, ProcessGetDurationToLock(curProcess), current_process_id);
            }
            curProcess = priorProcess;
            current_process_id = idx_prior;
            HeapPop(&ready);
        }

        if (current_process_id != -1)
            printf("%d %s\n", time, curProcess->name);
        else
            printf("%d IDLE\n", time);

        int time_to_event = quantum;


        if (current_process_id != -1)
        {
            int time_to_lock = ProcessGetDurationToLock(curProcess);
            if (time_to_event == -1 ||
                    time_to_lock < time_to_event)
                time_to_event = time_to_lock;
            ProcessExecute(curProcess, time_to_event);
        }

        if (current_process_id != -1 && ProcessGetDurationToLock(curProcess) == 0)
        {
            if (curProcess->next_lock < curProcess->locks->size)
            {
                locked = HeapInsert(locked, time + time_to_event +
                        ProcessGetDurationToReady(curProcess), current_process_id);
            }
            current_process_id = -1;
        }

        time += time_to_event;
    }
    return 0;
}