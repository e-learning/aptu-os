#include "common.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>

void ls_run()
{
	char 			files[256][256];
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
			strcpy( files[counter], entry->d_name);
			if ((length + strlen(files[counter])) > w.ws_col)
			{
				length = 0;
				printf("\n%s\t", files[counter]);
			}
			else
				printf("%s\t", files[counter]);

			length += strlen(files[counter]) + 4;		
			counter++;
		}
	}
	printf("\nTotal number of files: %d\n", counter);	
}
