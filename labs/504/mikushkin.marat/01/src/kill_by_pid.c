#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "kill_by_pid.h"

int kill(pid_t pid, int sig);

void kill_by_pid(int pid) {
	if (kill(pid, 9) == -1) {
		perror("");
	}
}
