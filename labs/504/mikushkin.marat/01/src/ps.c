#include "ps.h"

void ps() {
	DIR * directory;

	if ((directory = opendir("/proc"))) {
		struct dirent * content;

		while ((content = readdir(directory))) {
			if (atoi(content->d_name)) {
				char process_path[MAX_LEN_PROC_PATH];
				strcpy(process_path, "/proc/");
				strcat(process_path, content->d_name);
				strcat(process_path, "/comm");

				char process_name[MAX_LEN_PROC_NAME];
				FILE * f = fopen(process_path, "r");
				fgets(process_name, MAX_LEN_PROC_NAME, f);

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
