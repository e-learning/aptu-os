#include <stdio.h>
#include <limits.h>

#include "scheduler.h"


#define MAX_TASK_COUNT 1000
#define MAX_IO_COUNT 100

enum State
{
	waiting,
	completed,
	ready,
	io,
	runing
};

struct IO_operation
{
	unsigned iot;
	unsigned iol;
};

struct Task
{
	char name[255];
	unsigned ti;
	unsigned tw;
	unsigned io_count;
	unsigned io_num;
	struct IO_operation io_ops[MAX_IO_COUNT];
	unsigned prog;
	enum State status;
};

unsigned C;
unsigned tasks_count;
struct Task tasks[MAX_TASK_COUNT];
unsigned current_time;

void init_task(FILE* input, struct Task* task)
{
	int result;
	unsigned j = 0;
	fscanf(input, "%s %u %u", task->name, &(task->ti), &(task->tw));
	while (1)
	{
		result = fscanf(input, "%u %u", &(task->io_ops[j].iot),
										&(task->io_ops[j].iol));
		if (result == 0 || result == EOF) break;
		j++;
	}
	task->io_num = 0;
	task->io_count = j;
	task->prog = 0;
	task->status = waiting;
}

void load_tasks(FILE* input)
{
	unsigned i = 0;
	fscanf(input, "%u", &C);
	while (!feof(input))
	{
		init_task(input, &(tasks[i]));
		i++;
	}
	tasks_count = i;
}

// void debug_output()
// {
// 	int i;
// 	int j;
// 	for (i = 0; i < tasks_count; i++)
// 	{
// 		printf("%s %u %u ", tasks[i].name, tasks[i].ti, tasks[i].tw);
// 		for (j = 0; j < tasks[i].io_count; j++)
// 		{
// 			printf("%u %u ", tasks[i].io_ops[j].iot, tasks[i].io_ops[j].iol);
// 		}
// 		printf("\n");
// 	}
// }

void update_statuses(unsigned time)
{
	int i;
	unsigned io_end;
	struct Task* task;
	for (i = 0; i < tasks_count; i++)
	{
		task = &(tasks[i]);
		switch (task->status)
		{
			case waiting:
			{
				if (time >= task->ti)
				{
					task->status = ready;
				}
				break;
			}
			case io:
			{
				io_end = task->io_ops[task->io_num].iot + task->io_ops[task->io_num].iol;
				if (task->prog == io_end)
				{
					if (task->io_num < task->io_count)
						task->io_num++;
					task->status = ready;
				}
				break;
			}
			case runing:
			{
				if (task->prog == task->tw)
				{
					task->status = completed;
				} else if (task->io_num < task->io_count && 
							task->prog == task->io_ops[task->io_num].iot)
				{
					task->status = io;
				} else
				{
					task->status = ready;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

struct Task* select_task(unsigned time)
{
	int i;
	unsigned dur;
	unsigned min_dur = UINT_MAX;
	struct Task* task;
	struct Task* selected_task = 0;
	for (i = 0; i < tasks_count; i++)
	{
		task = &(tasks[i]);
		if (task->status != ready)	continue;
		if (task->io_num < task->io_count)
		{
			dur = task->io_ops[tasks->io_num].iot - task->prog;
		}
		else
		{
			dur = task->tw - task->prog;
		}
		if (dur < min_dur)
		{
			min_dur = dur;
			selected_task = task;
		}
	}
	return selected_task;
}

void update_io_tasks(unsigned dur)
{
	int i;
	unsigned io_end;
	struct Task* task;
	for (i = 0; i < tasks_count; i++)
	{
		task = &(tasks[i]);
		if (task->status != io) continue;
		io_end = task->io_ops[task->io_num].iot + task->io_ops[task->io_num].iol;
		if (task->prog + dur < io_end)
		{
			task->prog+=dur;
		}
		else
		{
			task->prog = io_end;
		}
	}
}

unsigned run_task(FILE* output, unsigned time, struct Task* task)
{
	unsigned dur;
	fprintf(output, "%u %s\n", time, task->name);
	if (task->io_num < task->io_count)
	{
		dur = task->io_ops[task->io_num].iot - task->prog;
	}
	else
	{
		dur = task->tw - task->prog;
	}
	if (dur > C) dur = C;
	task->prog += dur;
	task->status = runing;
	update_io_tasks(dur);
	return dur;
}

unsigned idle(FILE* output, unsigned time)
{
	unsigned min_dur = UINT_MAX;
	unsigned dur;
	unsigned io_end;
	int i;
	struct Task* task;
	fprintf(output, "%u IDLE\n", time);
	for (i = 0; i < tasks_count; i++)
	{
		task = &(tasks[i]);
		switch (task->status)
		{
			case waiting:
			{
				dur = task->ti - time;
				break;
			}
			case io:
			{
				io_end = task->io_ops[task->io_num].iot + task->io_ops[task->io_num].iol;
				dur = io_end - task->prog;
				break;
			}
			default:
			{
				dur = UINT_MAX;
				break;
			}
		}
		if (dur < min_dur)
		{
			min_dur = dur;
		}
	}
	update_io_tasks(dur);
	return dur;
}

int all_tasks_complited()
{
	int i;
	for (i = 0; i < tasks_count; i++)
	{
		if (tasks[i].status != completed)
			return 0;
	}
	return 1;
}

void run_scheduler(FILE* output)
{
	struct Task* task;
	unsigned current_time = 0;
	while (1)
	{
		update_statuses(current_time);
		if (all_tasks_complited()) break;
		task=select_task(current_time);
		if (task)
		{
			current_time+=run_task(output, current_time, task);
		}
		else
		{
			current_time+=idle(output, current_time);
		}
	}
}