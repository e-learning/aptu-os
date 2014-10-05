#include "common.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_ARGS_LENGTH 255
#define MAX_ARG_LENGTH 32

int child( const char *name, char **argv)
{
	printf("Now there is process PID:%d NAME:%s\n",getpid(), name); 
	return execvp(name, argv);
}

void free_memory(char **argv, int argc)
{
	int i = 0;
	for (i = 0; i <= argc; i++)
		free(argv[i]);
	free(argv);
	
}

char **copy_args(int argc, char *command_name)
{
	int 		i;
	char 		*substring;
	const char	delimiters[] = " ";
	
	char **args = (char**)calloc(argc + 1, sizeof(char*));
	if (!args)
		return (char **)NULL;
		
	substring = strtok(command_name, delimiters);
	args[0] = strdup(substring);
	//printf("ARGS:<%s> ", args[0]);
		
	for (i = 1; i < argc; i++)
	{
		substring = strtok(NULL, delimiters);
		args[i] = (char *)calloc(MAX_ARG_LENGTH, sizeof(char));
		strncpy(args[i], substring, strlen(substring) - 1);
		//printf("<%s> - <%d> ", args[i], strlen(args[i]));
	}
	
	args[argc] = (char *)NULL;
	printf("\n");
	return args;
}

int parse_string_for_argv(char *arguments, int size)
{
	int 		counter = 0;
	int			i = 0;
	
	if (arguments != NULL)
		counter = 1;
		
	for (i = 0; i < size; i++)
	{
		if (arguments[i] == ' ')
			counter++;
	}
	
	return counter;
}

void try_run_programm(char *command_name)
{
	pid_t pid;
	int status;
	char arguments[256];
	char *name;
	int was_malloced = 0;
	strcpy(arguments, command_name);
	int argc = parse_string_for_argv(arguments, strlen(arguments));
	if (argc == 1)
	{
		name = (char*)calloc(MAX_ARG_LENGTH, 1);
		was_malloced = 1;
		strncpy(name, command_name, strlen(command_name) - 1);
	}
	else
		name = strtok(command_name, " ");
	char **args = copy_args(argc, arguments);
	if (!args)
	{
		perror("cannot allocate");
		return;
	}
	
	pid = fork();
	if (pid < 0)
	{
		perror("cannot fork child process");
		free_memory(args, argc);
		return;
	}
		
	if ((pid == 0) && child(name, args))
	{
		perror("cannot exec child process");
		free_memory(args, argc);
		return;
	}
	
	waitpid(pid, &status, 0);
	free_memory(args, argc);
	if (was_malloced == 1)
		free(name);
}
