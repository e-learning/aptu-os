#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int parseInput(char **argv) {
	char * cstr;
	char arg[20];
	int argc = 0;

	while (scanf("%s", arg) != EOF) {
		if (strcmp(arg, "exit") == 0) {
			exit(0);
		}

		cstr = (char *) malloc (strlen(arg) + 1);
		strcpy(cstr, arg);
		argv[argc++] = cstr;
		if (getchar() == '\n') {
			break;
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

		execvp(argv[0], argv);
		perror("execvp error");

	} else if (!foundAmpersand) {
		waitpid(pid, NULL, 0);
	}
}

int main() {
	char * argv[256];
	int argc;

	while (1) {
		printf("$ ");
		argc = parseInput(argv);
		execute(argc, argv);
		for (int i = 0; i < argc; i++) {
			argv[i] = NULL;
		}
	}

	return 0;
}
