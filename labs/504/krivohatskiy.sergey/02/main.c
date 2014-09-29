#include <stdio.h>
#include "sheduler_emulator.h"
#include <malloc.h>
#include <string.h>
#define EMULATE_FAIL -1

char *my_getline(int *was_eof) {
	char * line = malloc(100), *linep = line;
	size_t lenmax = 100, len = lenmax;
	int c;

	if (line == NULL)
		return NULL;

	for (;;) {
		c = fgetc(stdin);
		if (c == EOF)
		{
			*was_eof = 1;
			break;
		}

		if (--len == 0) {
			len = lenmax;
			char * linen = realloc(linep, lenmax *= 2);

			if (linen == NULL) {
				free(linep);
				return NULL;
			}
			line = linen + (line - linep);
			linep = linen;
		}

		if ((*line++ = c) == '\n')
			break;
	}
	*line = '\0';
	return linep;
}

struct sheduler_task_list
{
    struct sheduler_task task;
    struct sheduler_task_list *next;
};

struct io_descr
{
    int descr[2];
    struct io_descr *next;
};

struct sheduler_task parse_task(char *line)
{
    struct sheduler_task task;
    task.next_io = 0;
    task.processed_time = 0;
    int len, tmp1, tmp2, descrs_count;
    char *id = line;
    line = strchr(line, ' ');
    *line = 0;
    line += 1;
    task.id = malloc(line - id + 1);
    memcpy(task.id, id, line - id + 1);
    sscanf(line, "%d %d%n", &task.when_getted, &task.time_to_process, &len);
    line += len;
    struct io_descr descrs;
    struct io_descr *last, *current;
    last = &descrs;
    last->next = 0;
    descrs_count = 0;
    while (sscanf(line, "%d %d%n", &tmp1, &tmp2, &len) == 2) {
        line += len;
        current = malloc(sizeof(struct io_descr));
        last->next = current;
        last = current;
        current->next = 0;
        current->descr[0] = tmp1;
        current->descr[1] = tmp2;
        ++descrs_count;
    }
    task.io_count = descrs_count;
    task.io_descriptors = (int*)malloc(descrs_count * sizeof(int) * 2);
    current = descrs.next;
    int i;
    for (i = 0; i < descrs_count; ++i)
    {
        task.io_descriptors[i * 2] = current->descr[0];
        task.io_descriptors[i * 2 + 1] = current->descr[1];
        last = current;
        current = current->next;
        free(last);
    }
    return task;
}

int main(void)
{
    int proc_time;

    if (!scanf("%d\n", &proc_time))
    {
        return -1;
    }

	int tasks_count = 0;
	struct sheduler_task_list tasks_list;
	struct sheduler_task_list *tasks_list_node;
	struct sheduler_task_list *tasks_list_last = &tasks_list;
	tasks_list.next = 0;
	int was_eof, len;
    was_eof = 0;
	do
	{
        char *line = my_getline(&was_eof);
		if (!line || !(len = strlen(line)))
		{
			free(line);
			break;
		}
		tasks_list_node = (struct sheduler_task_list *)malloc(sizeof(struct sheduler_task_list));
        tasks_list_node->next = 0;
        tasks_list_last->next = tasks_list_node;
        tasks_list_node->task = parse_task(line);
        tasks_list_last = tasks_list_node;

		++tasks_count;
		free(line);
	} while (!was_eof);

    struct sheduler_task *tasks = (struct sheduler_task *)malloc(tasks_count * sizeof(struct sheduler_task));
    int i;
    tasks_list_node = tasks_list.next;
    for (i = 0; i < tasks_count; ++i)
    {
        tasks[i] = tasks_list_node->task;
        tasks_list_last = tasks_list_node;
        tasks_list_node = tasks_list_node->next;
        free(tasks_list_last);
    }

    struct sheduler_out sh_out = emulate_sheduler(tasks, tasks_count, proc_time);

    struct sheduler_out *sh_out_next = &sh_out;
    struct sheduler_out *sh_out_last = 0;
    while ((sh_out_next = sh_out_next->next))
    {
        free(sh_out_last);
        printf("%d %s\n", sh_out_next->time, sh_out_next->sheduler_task);
        sh_out_last = sh_out_next;
    }
    free(sh_out_last);
    for (i = 0; i < tasks_count; ++i)
    {
        free(tasks[i].io_descriptors);
        free(tasks[i].id);
    }
    free(tasks);

    return 0;
}
