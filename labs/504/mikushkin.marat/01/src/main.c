#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>

#define MAX_USER_COMMAND_LEN 100
#define EXEC_SUCCESS 0
#define EXEC_FAILURE -1
#define MAX_LEN_PROC_PATH 100
#define MAX_LEN_PROC_NAME 100

char * get_current_dir_name();

int parse_input(char ** argv) {
	char user_input[MAX_USER_COMMAND_LEN];

	if (fgets(user_input, MAX_USER_COMMAND_LEN, stdin) == NULL) {
		return EXEC_FAILURE;
	}

	if (strcmp(user_input, "\n") == 0) {
		return EXEC_SUCCESS;
	}

	char * token = strtok(user_input, " ");
	int argc = 0;
	for (; token != NULL; token = strtok(NULL, " ")) {
		if (token[strlen(token) - 1] == '\n') {
			token[strlen(token) - 1] = '\0';
		}
		argv[argc] = (char *) malloc(strlen(token) + 1);
		strcpy(argv[argc++], token);
		if (strcmp(argv[argc - 1], "exit") == 0) {
			exit(0);
		}
	}
	argv[argc] = NULL;
	return argc;
}

void execute(int argc, char ** argv) {
	pid_t pid;
	int found_ampersand = (strcmp(argv[argc - 1], "&") == 0);

	pid = fork();

	if (pid < 0) {
		perror("Error: pid < 0");
	} else if (pid == 0) {
		if (found_ampersand) {
			argv[argc - 1] = NULL;
			argc--;
		}

		if (execvp(argv[0], argv) == -1) {
			exit(0);
		}

	} else if (!found_ampersand) {
		waitpid(pid, NULL, 0);
	}
}

int ls() {
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
		return EXEC_FAILURE;
	}
	return EXEC_SUCCESS;
}

int pwd() {
	printf("%s\n", get_current_dir_name());
	return EXEC_SUCCESS;
}

int ps() {
	DIR * directory;

	if ((directory = opendir("/proc"))) {
		struct dirent * content;

		while ((content = readdir(directory))) {
			if (atoi(content->d_name)) {
				char process_path[MAX_LEN_PROC_PATH];
				strcpy(process_path, "/proc/");
				strcat(process_path, content->d_name);
				strcat(process_path, "/comm");

				char process_name[MAX_LEN_PROC_NAME];
				FILE * f = fopen(process_path, "r");
				fgets(process_name, MAX_LEN_PROC_NAME, f);

				printf("%s:  %s", content->d_name, process_name);

				fclose(f);
			}
		}
		free(content);
		closedir(directory);
	} else {
		free(directory);
		return EXEC_FAILURE;
	}

	return EXEC_SUCCESS;
}

int kill_by_pid(int pid) {
	if (kill(pid, 9) == -1) {
		return EXEC_FAILURE;
	}
	return EXEC_SUCCESS;
}

int main() {
    char * arguments[MAX_USER_COMMAND_LEN];

	while (1) {
		printf("$ ");

		int number_of_arguments = parse_input(arguments);

		if (strcmp(arguments[0], "ls") == 0) {
			ls();
		} else if (strcmp(arguments[0], "ps") == 0) {
			ps();
		} else if (strcmp(arguments[0], "pwd") == 0) {
			pwd();
		} else if (strcmp(arguments[0], "kill") == 0) {
			kill_by_pid(atoi(arguments[0]));
		} else {
			execute(number_of_arguments, arguments);
		}
	}

	return EXIT_SUCCESS;
}
