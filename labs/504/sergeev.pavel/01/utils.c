#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>

#include "utils.h"

#define MAX_PATH_LENGTH 256
#define MAX_PROC_NAME_LENGTH 256

struct util utils[UTILS_COUNT];

void init_utils()
{
	strcpy(utils[0].name, "ls");
	utils[0].fun = ls_util;
	strcpy(utils[1].name, "pwd");
	utils[1].fun = pwd_util;
	strcpy(utils[2].name, "ps");
	utils[2].fun = ps_util;
	strcpy(utils[3].name, "kill");
	utils[3].fun = kill_util;
	strcpy(utils[4].name, "exit");
	utils[4].fun = exit_util;
}

void ls_util(char* command)
{
	DIR* dir;
	struct dirent* entry;
	char* dir_name = ".";
	dir = opendir(dir_name);
	if (!dir)
	{
		printf("Cannot open directory.\n");
	}
	while (1)
	{
		entry = readdir(dir);
		if (!entry)
		{
			break;
		}
		if ((strcmp(entry->d_name, ".") != 0) && 
			(strcmp(entry->d_name, "..") !=0 ))
		{
			printf("%s\n", entry->d_name);
		}
	}
	if (closedir(dir))
	{
		printf("Cannot close directory.\n");
	}
}

void pwd_util(char* command)
{
	char buf[MAX_PATH_LENGTH];
	if (getcwd(buf, MAX_PATH_LENGTH))
	{
		printf("%s\n", buf);
	}
	else
	{
		printf("PWD error.\n");
	}
}

void print_proc_info(unsigned num)
{
	FILE* stat;
	char buf[MAX_PATH_LENGTH];
	unsigned pid;
	char proc_name[MAX_PROC_NAME_LENGTH];
	sprintf(buf, "/proc/%u/stat", num);
	stat = fopen(buf, "r");
	if (!stat)
	{
		printf("Error when reading /proc/stat.\n");
		return;
	}
	fscanf(stat, "%u%s", &pid, proc_name);
	printf("%u %s\n", pid, proc_name);
	fclose(stat);
}

void ps_util(char* command)
{
	DIR* dir;
	struct dirent* entry;
	char* dir_name = "/proc";
	unsigned pid;
	dir = opendir(dir_name);
	if (!dir)
	{
		printf("Error when opening /proc.\n");
	}
	while (1)
	{
		entry = readdir(dir);
		if (!entry)
		{
			break;
		}
		if (sscanf(entry->d_name, "%u", &pid))
		{
			print_proc_info(pid);
		}
	}
	if (closedir(dir))
	{
		printf("Error when closing /proc.\n");
	}
}

void kill_util(char* command)
{
	unsigned pid;
	unsigned signal;
	if (sscanf(command, "kill%u%u", &pid, &signal))
	{
		if (kill(pid, signal))
		{
			printf("kill error.\n");
		}
	}
	else
	{
		printf("kill wrong arguments.\n");
	}
}

void exit_util(char* command)
{
	exit(EXIT_SUCCESS);
}