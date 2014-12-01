#include "pipes.h"

void pipes()
{
	pid_t pid,pid1;
	int pip_fd[2];

	pipe(pip_fd);

	pid = fork();
	if(pid == 0) {
		close(1);
		dup(pip_fd[1]);
		execlp(cmd1,cmd1,NULL);
	} else {
		wait(pid);
		close(pip_fd[1]);
		pid1 = fork();

		if (pid1 == 0) {
			close(0);
			dup(pip_fd[0]);
			execlp(cmd2,cmd2, NULL);
		}
		else
			wait(pid1);
	}
}
