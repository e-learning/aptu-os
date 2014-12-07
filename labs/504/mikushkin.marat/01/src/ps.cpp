#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include "ps.h"

void ps() {
	DIR * directory;
	if ((directory = opendir("/proc"))) {
		struct dirent * content;

		while ((content = readdir(directory))) {
			if (atoi(content->d_name)) {
				char process_path[100];
				strcpy(process_path, "/proc/");
				strcat(process_path, content->d_name);
				strcat(process_path, "/comm");

				char process_name[100];
				FILE * f = fopen(process_path, "r");
				fgets(process_name, 100, f);

				printf("%s:  %s", content->d_name, process_name);

				fclose(f);
			}
		}
		free(content);
		closedir(directory);
	} else {
		free(directory);
		perror("");
	}
}

