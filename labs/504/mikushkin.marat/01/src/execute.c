#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "execute.h"
#include "pwd.h"
#include "ls.h"
#include "ps.h"

void execute1(char * cmd1)
{
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		execlp(cmd1,cmd1,NULL);
	}
	else {
		wait(pid);
	}
}

void execute2(char * cmd1, char * cmd2)
{
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		execlp(cmd1,cmd1,cmd2,NULL);
	}
	else {
		wait(pid);
	}
}
