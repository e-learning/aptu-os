#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "cmd_interface.h"


#define MAX_COMMAND_SIZE 20


void init()
{
	int N;
	if (scanf("%d\n", &N) != 1)
	{
		printf("cmd_init error\n");
		exit(1);
	}
	init_allocator(N);
}


void run_alloc(unsigned size)
{
	void* ptr = alloc_mem(size);
	if (ptr == NULL)
	{
		printf("-\n");
	}
	else
	{
		printf("+ %d\n", to_local_offset(ptr));
	}
}


void run_free(unsigned offset)
{
	void* ptr = from_local_offset(offset);
	if (free_mem(ptr) == 0)
	{
		printf("+\n");
	}
	else
	{
		printf("-\n");
	}
}


void run_info()
{
	struct stat stat_info = info();
	printf("%u %u %u\n",
			stat_info.blk_count,
			stat_info.allocated_memory,
			stat_info.max_successful_alloc);

}


void run_map()
{
	char* memory_map = map();
	printf("%s\n", memory_map);
	free(memory_map);
}


void parse_command(char* command)
{
	unsigned size;
	unsigned offset;
	char* token = strtok(command, " \n");
	if (token == NULL)
	{
		return;
	}
	if (strcmp(token, "ALLOC") == 0)
	{
		token = strtok(NULL, " \n");
		if (token == NULL || sscanf(token, "%u", &size) != 1)
		{
			printf("Wrong alloc argument\n");
		}
		else
		{
			run_alloc(size);
		}
	}
	else if (strcmp(token, "FREE") == 0)
	{
		token = strtok(NULL, " \n");
		if (token == NULL || sscanf(token, "%u", &offset) != 1)
		{
			printf("Wrong free argument\n");
		}
		else
		{
			run_free(offset);
		}
	}
	else if (strcmp(token, "INFO") == 0)
	{
		run_info();
	}
	else if (strcmp(token, "MAP") == 0)
	{
		run_map();
	}
	else
	{
		printf("Unrecognized command\n");
	}
}


void run_cmd_interface()
{
	char command[MAX_COMMAND_SIZE];
	init();
	while (!feof(stdin))
	{
		command[0] = '\0';
		fgets(command, MAX_COMMAND_SIZE, stdin);
		parse_command(command);
	}
}