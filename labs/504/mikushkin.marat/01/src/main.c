#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const int MAX_USER_COMMAND_LENGTH = 256;

int parseInput(char ** argv) {
	char userInput[MAX_USER_COMMAND_LENGTH];

	if (fgets(userInput, MAX_USER_COMMAND_LENGTH, stdin) == NULL) {
		return -1;
	}

	if (strcmp(userInput, "\n") == 0) {
		return 0;
	}

	char * token = strtok(userInput, " ");
	int argc = 0;
	for (; token != NULL; token = strtok(NULL, " ")) {
		if (token[strlen(token) - 1] == '\n') {
			token[strlen(token) - 1] = '\0';
		}
		argv[argc] = (char *) malloc (strlen(token) + 1);
		strcpy(argv[argc++], token);
		if (strcmp(argv[argc - 1], "exit") == 0) {
			exit(0);
		}
	}
	argv[argc] = NULL;
	return argc;
}

void execute(int argc, char** argv) {
	pid_t pid;
	int foundAmpersand = (strcmp(argv[argc - 1], "&") == 0);

	pid = fork();

	if (pid < 0) {
		perror("Error (pid < 0)");
	} else if (pid == 0) {
		if (foundAmpersand) {
			argv[argc - 1] = NULL;
			argc--;
		}

		if (execvp(argv[0], argv) == -1) {
			exit(0);
		}

	} else if (!foundAmpersand) {
		waitpid(pid, NULL, 0);
	}
}

int main() {
	char * partsOfUserCommand[256];

	while (1) {
		printf("$ ");
		int numberOfPartsInUserCommand = parseInput(partsOfUserCommand);
		execute(numberOfPartsInUserCommand, partsOfUserCommand);
	}

	return 0;
}
