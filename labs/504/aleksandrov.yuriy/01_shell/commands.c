#include "commands.h"

#include <stdio.h> // stderr
#include <sys/types.h> // opendir, closedir, scandir
#include <dirent.h> // struct dirent
#include <stdlib.h> // free
#include <sys/ioctl.h> // ioctl() - terminal size
#include <string.h> // strlen
#include <unistd.h> // getcwd 
#include <signal.h>

#define KBLU  "\x1B[34m" // blue text
#define RESET "\033[0m" // normal color (white) text
#define ENBOLD "\033[1m" // enable bold
#define DISBOLD "\033[0m" // disable bold


#define LONGSPACESTR "                                          "

void exec_ls(char ** arguments)
{
	char* dirname;
	if (arguments[0] != NULL)
		dirname = arguments[0];
	else
		dirname = ".";
	int entries_num; 
	int entry = 0;
	struct dirent **namelist; // entries
	
	
	entries_num = scandir(dirname, &namelist, NULL, alphasort);
    if (entries_num < 0)
    {
		fprintf(stderr, "ls: cannot scan %s\n", dirname);
    }
	else
	{
		// find maximal name length
		int maxlen = 0, tmplen;
		while(entry < entries_num)
		{
			tmplen = strlen(namelist[entry++] -> d_name);
			if (tmplen > maxlen)
				maxlen = tmplen;
		}
		
		
		struct winsize w;
		ioctl(0, TIOCGWINSZ, &w);
		int col_num = w.ws_col / (maxlen + 2);
		
		// print names
		int entries_row = 0;
		int entries_rows_num = (entries_num % col_num == 0)?  entries_num / col_num : entries_num / col_num + 1;
		int col;
		while(entries_row < entries_rows_num)
		{
			col = 0;
			entry = 2+entries_row++;
			while (col++ < col_num && entry < entries_num)
			{
				switch (namelist[entry] -> d_type)
				{
					case DT_DIR:
					// highlight directories
						printf("%s%s%s%s%s%.*s", KBLU, ENBOLD, namelist[entry] -> d_name, 
						       DISBOLD, RESET, (int)(maxlen+2-strlen(namelist[entry] -> d_name)), LONGSPACESTR);
						break;
					default:
						printf("%s%.*s", namelist[entry] -> d_name, (int)(maxlen+2-strlen(namelist[entry] -> d_name)), LONGSPACESTR);
				}
				free(namelist[entry]);
				entry += entries_rows_num;
				
			}
			printf("\n");
		}
        
		free(namelist);
	}
}



void exec_pwd()
{
	char path[1024];
	if (getcwd(path, sizeof(path)) != NULL)
		fprintf(stdout, "%s\n", path);
	else
		perror("getcwd() error");
}

void exec_kill(char ** arglist)
{
	if (arglist[0] == NULL)
	{
		fprintf(stderr, "Error: no arguments. Use as `kill -signal pid`\n");
		return;
	}
	else if (arglist[0][0] == '-')
	{
		if(arglist[1] == NULL)
		{
			fprintf(stderr, "Error: no pid. Use as `kill -signal pid`\n");
			return;
		}
			
		int signal =  atoi(&arglist[0][1]);
		int pid = atoi(arglist[1]);
		if (kill(pid, signal) !=0)
		{
			fprintf(stderr, "Error: cannot kill %d proccess\n", pid);
			return;
		}
			
		
	} else if(arglist[0] == NULL)
	{
		int pid = atoi(arglist[1]);
		if (kill(pid, 9) !=0)
		{
			fprintf(stderr, "Error: cannot kill %d proccess\n", pid);
			return;
		}
	}
}


