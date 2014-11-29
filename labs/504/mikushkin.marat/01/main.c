#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>
#include "constants.h"
#include "ps.h"

char * get_current_dir_name();
int kill(pid_t pid, int sig);

void ls();
void pwd();
//void ps();
void kill_by_pid(int pid);

void handler() {
    puts("\nCtrl-C was pressed.");
}


int parse_input(char ** arguments) {
	char user_input[MAX_USER_COMMAND_LEN];

	if (fgets(user_input, MAX_USER_COMMAND_LEN, stdin) == NULL) {
		return -1;
	}

	if (strcmp(user_input, "\n") == 0) {
		return 0;
	}

	char * token = strtok(user_input, " ");
	int number_of_arguments = 0;
	for (; token != NULL; token = strtok(NULL, " ")) {
		if (token[strlen(token) - 1] == '\n') {
			token[strlen(token) - 1] = '\0';
		}
		arguments[number_of_arguments] = (char *) malloc(strlen(token) + 1);
		strcpy(arguments[number_of_arguments++], token);
		if (strcmp(arguments[number_of_arguments - 1], "exit") == 0) {
			exit(0);
		}
	}
	arguments[number_of_arguments] = NULL;
	free(token);
	return number_of_arguments;
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

void ls() {
	DIR * directory;

	if ((directory = opendir("."))) {
		struct dirent * content;

		while ((content = readdir(directory))) {
			if (strcmp(content->d_name, ".") != 0 && strcmp(content->d_name, "..") != 0) {
				printf("%s\n", content->d_name);
			}
		}
		closedir(directory);
		free(content);
	} else {
		free(directory);
		perror("");
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
