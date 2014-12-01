#include "dir.h"

void out_dir()
{
	pid_t pid;
	char buf[BUFSIZE];
	int fd;

	pid = fork();
	if (pid == 0) {
		fd = open(cmd2, O_WRONLY|O_CREAT|O_TRUNC, 0644);
		close(1);
		dup(fd);
		execlp(cmd1,cmd1,NULL);
	} else {
		wait();
	}
}
void in_dir()
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
	else
		wait(pid);
}
