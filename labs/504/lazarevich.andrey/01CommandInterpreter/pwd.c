#include "common.h"
#define MAX_PATH_SIZE 256

void pwd_run()
{
	char pwd[MAX_PATH_SIZE];
	if (getcwd(pwd, MAX_PATH_SIZE) == NULL)
	{
		perror("getcwd");
		exit(1);
	}
	else
		printf("%s\n", pwd);
}
