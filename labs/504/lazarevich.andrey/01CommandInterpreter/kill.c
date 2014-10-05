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
	if (substring != NULL)
		return atoi(substring);
	else
		return 0;
}

void kill_run(char *command_name)
{
	int pid = 0;

	pid = parse_command_for_pid(command_name);
	if (pid == 0)
		printf("You wrote pid = 0 or not a number at all\n");
	else if (kill(pid, SIGTERM) == 0)
		printf("Process with PID: %d succesfully killed\n", pid);
	else
		printf("Cannot kill process with PID: %d or it doesn't exist\n", pid);
}
