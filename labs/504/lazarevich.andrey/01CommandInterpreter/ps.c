#include "common.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#define FILE_NAME_MAX_LENGTH 256
#define DIR_NAME_MAX_LENGTH 256
#define PROC_NAME_MAX_LENGTH 32

void show_process_info(FILE *fp)
{
	int 	pid = 0;
	char	name[PROC_NAME_MAX_LENGTH];
	//printf("I'm opened\n");	
	fscanf(fp, "%d %s", &pid, name);
	if (pid != 0 && name != NULL)
		printf("%d\t%s\n", pid, name);
}

void ps_run()
{
	const char		*status_file = "stat";
	const char		*base_dir = "/proc/";
	DIR 			*dir;
	DIR				*subdir;
	struct dirent	*entry;
	FILE			*fp;
	char			file_name[FILE_NAME_MAX_LENGTH];
	char			dir_name[DIR_NAME_MAX_LENGTH];
	
	dir = opendir(base_dir);
	if (!dir)
	{
		perror("opendir");
		exit(1);
	}
	printf("PID \t NAME\n");
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
		{
			strcpy(dir_name, base_dir);
			strcat(dir_name, entry->d_name);
			subdir = opendir(dir_name);
			if (subdir)
			{
				strcpy(file_name, dir_name);
				strcat(file_name, "/");
				strcat(file_name, status_file);
				
				fp = fopen( file_name, "r");
				if (fp != NULL)
					show_process_info(fp);				
			}	
		}
	}
}
