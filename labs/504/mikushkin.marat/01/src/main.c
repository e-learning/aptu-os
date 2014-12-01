#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "constants.h"
#include "ps.h"
#include "ls.h"
#include "parse_input.h"

char * get_current_dir_name();
int kill(pid_t pid, int sig);

void pwd();
//void ps();
void kill_by_pid(int pid);

void handler() {
    puts("\nCtrl-C was pressed.");
}

void execute(int number_of_arguments, char ** arguments) {
	if (number_of_arguments == 0) {
		return;
	}

	if (strcmp(arguments[0], "ls") == 0) {
		ls();
		return;
	} else if (strcmp(arguments[0], "ps") == 0) {
		ps();
		return;
	} else if (strcmp(arguments[0], "pwd") == 0) {
		pwd();
		return;
	} else if (strcmp(arguments[0], "kill") == 0 && number_of_arguments > 1) {
		int pid = atoi(arguments[1]);
		if (pid > 0) {
			kill_by_pid(pid);
		}
		return;
	}

	pid_t pid;
	int found_ampersand = (strcmp(arguments[number_of_arguments - 1], "&") == 0);
	pid = fork();
	if (pid < 0) {
		perror("Error: pid < 0");
	} else if (pid == 0) {
		if (found_ampersand) {
			arguments[number_of_arguments - 1] = NULL;
			number_of_arguments--;
		}
		if (execvp(arguments[0], arguments) == -1) {
			exit(0);
		}
	} else if (!found_ampersand) {
		waitpid(pid, NULL, 0);
	}
}

void pwd() {
	char * directory = get_current_dir_name();
	printf("%s\n", directory);
	free(directory);
}

void kill_by_pid(int pid) {
	if (kill(pid, 9) == -1) {
		perror("");
	}
}

int main() {
	signal(SIGINT, handler);

	char * arguments[MAX_USER_COMMAND_LEN];

	while (1) {
		printf("$ ");
		int number_of_arguments = parse_input(arguments);
		execute(number_of_arguments, arguments);

		for (int i = 0; i < number_of_arguments; ++i) {
			free(arguments[i]);
		}
	}

	return EXIT_SUCCESS;
}
