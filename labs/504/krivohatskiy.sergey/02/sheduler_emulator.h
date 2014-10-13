#ifndef SHEDULER_EMULATOR_H
#define SHEDULER_EMULATOR_H

struct sheduler_task
{
    char *id;
    int when_getted;
    int time_to_process;
    int processed_time;
    int io_count;
    int next_io;
    int *io_descriptors;
};

struct sheduler_out
{
    int time;
    char *sheduler_task;
    struct sheduler_out *next;
};

struct sheduler_out emulate_sheduler(struct sheduler_task *tasks, int tasks_count, int proc_time);

#endif // SHEDULER_EMULATOR_H
