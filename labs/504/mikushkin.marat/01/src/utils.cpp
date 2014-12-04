#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "utils.h"

ProcessCommunicationType parse(int numberOfArguments, char** arguments, char** command1, char** command2) {
	ProcessCommunicationType result = ANOTHER;

	int split = -1;

	for (int i = 0; i < numberOfArguments; i++) {
		if (strcmp(arguments[i], "|") == 0) {
			result = PIPE;
			split = i;

		} else if (strcmp(arguments[i], ">") == 0) {
			result = REDIRECT;
			split = i;
		}
	}

	if (result != ANOTHER) {
		for (int i = 0; i < split; i++) {
			command1[i] = arguments[i];
		}

		int count = 0;
		for (int i = split + 1; i < numberOfArguments; i++) {
			command2[count] = arguments[i];
			count++;
		}

		command1[split] = NULL;
		command2[count] = NULL;
	}

	return result;
}

void pipeCommand(char** command1, char** command2) {
	int fileDescriptors[2];
	pipe(fileDescriptors);
	pid_t processID;

	if (fork() == 0) {
		dup2(fileDescriptors[0], 0);

		close(fileDescriptors[1]);

		execvp(command2[0], command2);
		perror("Execution failed");

	} else if ((processID = fork()) == 0) {
		dup2(fileDescriptors[1], 1);

		close(fileDescriptors[0]);

		execvp(command1[0], command1);
		perror("Execution failed");

	} else {
		waitpid(processID, NULL, 0);
	}
}

int readArguments(char **arguments) {
	char *buffer;
	string argument;
	int numberOfArguments = 0;

	while (cin >> argument) {
		if (argument == "exit") {
			exit(0);
		}

		buffer = new char[argument.size() + 1];
		strcpy(buffer, argument.c_str());
		arguments[numberOfArguments] = buffer;

		numberOfArguments++;

		if (cin.get() == '\n') {
			break;
		}
	}

	arguments[numberOfArguments] = NULL;

	return numberOfArguments;
}

void redirectCommand(char** command, char** file) {
	int fileDescriptors[2];
	int count;
	int fileDescriptor;
	char tmpCharacter;
	pid_t processID;

	pipe(fileDescriptors);

	if (fork() == 0) {
		fileDescriptor = open(file[0], O_RDWR | O_CREAT, 0666);

		if (fileDescriptor < 0) {
			printf("Error: %s\n", strerror(errno));
			return;
		}

		dup2(fileDescriptors[0], 0);

		close(fileDescriptors[1]);

		while ((count = read(0, &tmpCharacter, 1)) > 0) {
			write(fileDescriptor, &tmpCharacter, 1);
		}

		//execlp("...", "...", NULL);
		exit(0);

	} else if ((processID = fork()) == 0) {
		dup2(fileDescriptors[1], 1);

		close(fileDescriptors[0]);

		execvp(command[0], command);
		perror("execvp failed");

	} else {
		waitpid(processID, NULL, 0);
		close(fileDescriptors[0]);
		close(fileDescriptors[1]);
	}
}

void runCommand(int numberOfArguments, char** arguments) {
	pid_t processID;
	const char *ampersand;
	ampersand = "&";
	bool isAmpersand = false;

	if (strcmp(arguments[numberOfArguments - 1], ampersand) == 0) {
		isAmpersand = true;
	}

	processID = fork();

	if (processID < 0) {
		perror("Error (pid < 0)");
	}

	else if (processID == 0) {
		if (isAmpersand) {
			arguments[numberOfArguments - 1] = NULL;
			numberOfArguments--;
		}

		if (strcmp(arguments[0], "exit") == 0) {
			exit(0);
			return;
		} else if (strcmp(arguments[0], "pwd") == 0) {
			char * directory = get_current_dir_name();
			printf("%s\n", directory);
			free(directory);
			exit(0);
			return;
		} else if (strcmp(arguments[0], "kill") == 0) {
			kill(atoi(arguments[1]), SIGTERM);
			exit(0);
			return;
		} else if (strcmp(arguments[0], "ls") == 0) {
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
			exit(0);
			return;
		} else if (strcmp(arguments[0], "ps") == 0) {
			DIR * directory;

				if ((directory = opendir("/proc"))) {
					struct dirent * content;

					while ((content = readdir(directory))) {
						if (atoi(content->d_name)) {
							char process_path[100];
							strcpy(process_path, "/proc/");
							strcat(process_path, content->d_name);
							strcat(process_path, "/comm");

							char process_name[100];
							FILE * f = fopen(process_path, "r");
							fgets(process_name, 100, f);

							printf("%s:  %s", content->d_name, process_name);

							fclose(f);
						}
					}
					free(content);
					closedir(directory);
				} else {
					free(directory);
					perror("");
				}

			exit(0);
			return;
		}

		execvp(arguments[0], arguments);
		perror("execvp error");

	} else if (!isAmpersand) {
		waitpid(processID, NULL, 0);
	}
}

void handle (int parameter) {
	puts("Bye!");
	exit(0);
}
