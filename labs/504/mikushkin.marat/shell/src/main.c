#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
/*
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
*/
int parseInputTwo(char ** argv) {
	char userInput[20];

	if (fgets(userInput, 20, stdin) == NULL) {
		return -1;
	}

	char * pch;
	char * cstr;
	pch = strtok(userInput, " ");
	int argc = 0;
	while (pch != NULL) {
		if (pch[strlen(pch) - 1] == '\n') {
			pch[strlen(pch) - 1] = '\0';
		}


		cstr = (char *) malloc (strlen(pch) + 1);
		strcpy(cstr, pch);

		if (strcmp(cstr, "exit") == 0) {
			exit(0);
		}

		argv[argc++] = cstr;
		pch = strtok(NULL, " ");
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

	while (1) {
		printf("$ ");
		//argc = parseInput(argv);
		int argc = parseInputTwo(argv);

		execute(argc, argv);
		/*


		for (int i = 0; i < argc; i++) {
			argv[i] = NULL;
		}
		*/
	}

	return 0;
}
