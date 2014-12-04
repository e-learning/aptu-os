#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "utils.h"

int main() {
	signal (SIGINT, handle);

	char *arguments[MAX_ARGS], *command1[MAX_ARGS], *command2[MAX_ARGS];
	ProcessCommunicationType pipeRedirect;
	int numberOfArguments;

	while (true) {
		cout << "$ ";
		numberOfArguments = readArguments(arguments);

		pipeRedirect = parse(numberOfArguments, arguments, command1, command2);

		if (pipeRedirect == PIPE) {
			pipeCommand(command1, command2);
		}
		else if (pipeRedirect == REDIRECT) {
			redirectCommand(command1, command2);
		}
		else {
			runCommand(numberOfArguments, arguments);
		}

		for (int i = 0; i < numberOfArguments; i++) {
			arguments[i] = NULL;
		}
	}

	return 0;
}
