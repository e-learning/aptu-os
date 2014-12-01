#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "execute.h"
#include "pwd.h"
#include "ls.h"
#include "ps.h"

void kill_by_pid(int pid);

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
