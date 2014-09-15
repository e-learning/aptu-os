#include "common.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>

void ls_run()
{
	DIR 			*dir;
	struct dirent	*entry;	
	struct winsize	w;
	int				counter = 0;
	size_t 			length = 0;
	
	ioctl(0, TIOCGWINSZ, &w);
	dir = opendir("./");
	if (!dir)
	{
		perror("opendir");
		exit(1);
	}
	
	while (	(entry = readdir(dir)) != NULL )
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{ 
			if ((length + strlen(entry->d_name)) >= w.ws_col)
			{
				length = 0;
				printf("\n%s\t", entry->d_name);
			}
			else
				printf("%s\t", entry->d_name);

			length += strlen(entry->d_name) + 5;		
			counter++;
		}
	}
	printf("\nTotal number of files: %d\n", counter);	
}
