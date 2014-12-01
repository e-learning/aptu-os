#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dir.h"
#include "constants.h"

void out_dir(char * cmd1, char * cmd2)
{
	pid_t pid;
	int fd;

	pid = fork();
	if (pid == 0) {
		fd = open(cmd2, O_WRONLY|O_CREAT|O_TRUNC, 0644);
		close(1);
		dup(fd);
		execlp(cmd1,cmd1,NULL);
	} else {
		//wait();
		wait(NULL);
	}
}

void in_dir(char * cmd1, char * cmd2)
{

	pid_t pid;
	int fd;


	pid = fork();
	if (pid == 0) {
		fd = open(cmd2, O_RDONLY);
		close(0);
		dup(fd);
		execlp(cmd1,cmd1,NULL);
		}
	else {
		wait(pid);
	}
}
