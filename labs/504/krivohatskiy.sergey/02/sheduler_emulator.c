#include "sheduler_emulator.h"
#include <stdlib.h>
#include "heap.h"


int cmp2(type l, type r)
{
    // It's save because of when_getted is garanteed to be >= 0
    return l.when_getted < r.when_getted;
}

int get_min(struct sheduler_task a)
{
    int min_a = a.time_to_process;
    int next_io_time;
    if (a.next_io != a.io_count)
    {
        next_io_time = a.io_descriptors[a.next_io * 2] - a.processed_time;
        min_a = min_a < next_io_time ? min_a : next_io_time;
    }
    return min_a;
}

int cmp1(type a, type b)
{
    return ((get_min(a)) < (get_min(b)));
}

struct sheduler_out emulate_sheduler(struct sheduler_task *tasks, int tasks_count, int proc_time)
{
    struct sheduler_out result;
    result.next = 0;
    int current_time = 0;

    struct sheduler_out *node, *last_node;
    last_node = &result;
    struct heap priority_que_tasks;
    heap_init(&priority_que_tasks);
    priority_que_tasks.cmp = &cmp2;
    int i;
    for (i = 0; i < tasks_count; ++i)
    {
        heap_push(&priority_que_tasks, tasks[i]);
    }
    struct heap priority_que;
    heap_init(&priority_que);
    priority_que.cmp = &cmp1;
    i = 0;
    while (priority_que_tasks.count || priority_que.count)
    {
        while (priority_que_tasks.count)
        {
            if (heap_front(&priority_que_tasks).when_getted > current_time)
            {
                break;
            }
            heap_push(&priority_que, heap_front(&priority_que_tasks));
            heap_pop(&priority_que_tasks);
        }

        if (!priority_que.count)
        {
            if (i != tasks_count)
            {
                node = (struct sheduler_out*)malloc(sizeof(struct sheduler_out));
                node->sheduler_task = "IDLE";
                node->time = current_time;
                last_node->next = node;
                node->next = 0;
                last_node = node;
                current_time = heap_front(&priority_que_tasks).when_getted;
            }
            continue;
        }

        struct sheduler_task task = heap_front(&priority_que);
        heap_pop(&priority_que);
        node = (struct sheduler_out*)malloc(sizeof(struct sheduler_out));
        node->sheduler_task = task.id;
        node->time = current_time;
        last_node->next = node;
        node->next = 0;
        last_node = node;
        int time_to_process = get_min(task);
        if (time_to_process > proc_time)
        {
            task.time_to_process -= proc_time;
            task.processed_time += proc_time;
            current_time += proc_time;
            heap_push(&priority_que, task);
        }
        else
        {
            current_time += time_to_process;
            task.processed_time += time_to_process;
            task.time_to_process -= time_to_process;
            if (task.time_to_process)
            {
                task.when_getted = current_time + task.io_descriptors[task.next_io * 2 + 1];
                task.processed_time += task.io_descriptors[task.next_io * 2 + 1];
                task.time_to_process -= task.io_descriptors[task.next_io * 2 + 1];
                task.next_io += 1;
                heap_push(&priority_que_tasks, task);
            }
        }
    }
    heap_term(&priority_que);
    heap_term(&priority_que_tasks);
    return result;
}
