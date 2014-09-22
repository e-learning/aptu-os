#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "utils.h"
#include "shell.h"

#define MAX_TOKENS_COUNT 20
#define MAX_TOKEN_SIZE 256

char* tokens[MAX_TOKENS_COUNT];
unsigned token_count;

void sigint_handler(int signum)
{
	printf("SIGINT received\n");
	exit(EXIT_SUCCESS);
}

void setup()
{
	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		printf("sigaction error\n");
		exit(EXIT_FAILURE);
	}
	init_utils(utils);
}

void run()
{
	char command[MAX_COMMAND_LENGTH];
	while (1)
	{
		printf(":>");
		command[0] = '\0';
		fgets(command, MAX_COMMAND_LENGTH, stdin);
		parse(command);
	}
}

void parse(char* command)
{
	int i;
	int matched = 0;
	char util_name[MAX_COMMAND_LENGTH];
	util_name[0] = '\0';
	if (sscanf(command, "%s", util_name))
	{
		for (i = 0; i < UTILS_COUNT; i++)
		{
			if (strcmp(util_name, utils[i].name) == 0)
			{
				matched = 1;
				utils[i].fun(command);
			}
		}
		if (!matched)
		{
			execute(command);
		}
	}
}

int create_tokens(char* command)
{
	token_count = 0;
	char copy_command[MAX_COMMAND_LENGTH];
	char* token;
	unsigned token_length;
	strcpy(copy_command, command);
	token = strtok(copy_command, " \n");
	while(token)
	{
		token_length = strlen(token);
		if (token_length > 1)
		{
			tokens[token_count] = (char*)malloc(token_length);
			strcpy(tokens[token_count], token);
			token_count++;
		}
		token = strtok(0, " \n");
	}
	tokens[token_count] = 0;
	return token_count;
}

void delete_tokens()
{
	int i;
	for (i = 0; i < token_count; i++)
	{
		free((void*)tokens[i]);
	}
	token_count = 0;
}

void create_process()
{
	unsigned pid = fork();
	int status;
	if (pid < 0)
	{
		printf("Fork error.\n");
		return;
	}
	if (pid == 0)
	{
		if (execvp(tokens[0], tokens))
		{
			printf("Exec error.\n");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}
	else
	{
		waitpid(pid, &status, 0);
	}
}

void execute(char* command)
{
	if (create_tokens(command))
	{
		create_process();
		delete_tokens();
	}
}