#include "common.h"
#include <string.h>
#include <signal.h>

int parse_command_for_pid(char *command_name)
{
	char 		*substring;
	const char	delimiters[] = " ";

	substring = strtok( command_name, delimiters);
	substring = strtok (NULL, delimiters);
	/* some magic above */ 	

	return atoi(substring);
}

void kill_run(char *command_name)
{
	int pid = 0;

	pid = parse_command_for_pid(command_name);

	if (kill(pid, SIGTERM) == 0)
		printf("Process with PID: %d succesfully killed\n", pid);
	else
		printf("Cannot kill process with PID: %d or it doesn't exist\n", pid);
}
