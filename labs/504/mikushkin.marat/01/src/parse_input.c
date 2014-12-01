#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse_input.h"
#include "constants.h"

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
