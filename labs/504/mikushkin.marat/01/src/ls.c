#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ls.h"

void ls() {
	DIR * directory;

	if ((directory = opendir("."))) {
		struct dirent * content;

		while ((content = readdir(directory))) {
			if (strcmp(content->d_name, ".") != 0 && strcmp(content->d_name, "..") != 0) {
				printf("%s\n", content->d_name);
			}
		}
		closedir(directory);
		free(content);
	} else {
		free(directory);
		perror("");
	}
}
