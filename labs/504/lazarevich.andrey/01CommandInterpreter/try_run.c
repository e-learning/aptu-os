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
	
	char **args = malloc(sizeof(char *) * (argc + 1));
	if (!args)
		return (char **)NULL;
		
	substring = strtok(command_name, delimiters);
	args[0] = strdup(substring);
	//printf("ARGS:<%s> ", args[0]);
		
	for (i = 1; i < argc; i++)
	{
		substring = strtok(NULL, delimiters);
		args[i] = malloc(sizeof(char) * MAX_ARG_LENGTH);
		strncpy(args[i], substring, strlen(substring) - 1);
		//printf("<%s> - <%d> ", args[i], strlen(args[i]));
	}
	
	args[argc] = (char *)NULL;
	//printf("\n");
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
	strcpy(arguments, command_name);
	name = strtok(command_name, " ");
	int argc = parse_string_for_argv(arguments, strlen(arguments));
	//printf("ARGC:%d\n", argc);
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
	
}
