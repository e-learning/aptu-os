#include <stdio.h>
#include <stdlib.h>
#include "pwd.h"

char * get_current_dir_name();

void pwd() {
	char * directory = get_current_dir_name();
	printf("%s\n", directory);
	free(directory);
}

