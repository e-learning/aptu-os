#include "commands.h"

#include <stdio.h> // stderr
#include <sys/types.h> // opendir, closedir, scandir
#include <dirent.h> // struct dirent
#include <stdlib.h> // free
#include <sys/ioctl.h> // ioctl() - terminal size
#include <string.h> // strlen
#include <unistd.h> // getcwd 
#include <signal.h>
#include <pwd.h>
#include <sys/stat.h>
#include <errno.h>

#define KBLU  "\x1B[34m" // blue text
#define RESET "\033[0m" // normal color (white) text
#define ENBOLD "\033[1m" // enable bold
#define DISBOLD "\033[0m" // disable bold
#define USER_WIDTH 20
#define PID_WIDTH 10


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
    if (entry == 0)
    {
		fprintf(stderr, "ls: cannot access %s: No such file or directory\n", dirname);
    }
	else if (entry < 0)
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
        
        fprintf(stderr, "now free namelist...\n");
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

void exec_ps()
{
    DIR *pd = opendir("/proc");
    if (!pd)
    {
        fprintf(stderr, "ps: Cannot access /proc\n");
        return;
    }

    struct dirent* dent = readdir(pd);
    struct stat user_info;
    printf("USER%.*sPID%.*sCOMMAND", USER_WIDTH-4, LONGSPACESTR, PID_WIDTH-3, LONGSPACESTR);
    while (dent != 0)
    {
        if (strcmp(dent->d_name, "self") == 0) {
            dent = readdir(pd);
            continue;
        }

        char user_info_fpath[PATH_MAX];
        char info_fpath[PATH_MAX];
        strcpy(user_info_fpath, "/proc/");
        strcat(user_info_fpath, dent->d_name);
        strcpy(info_fpath, user_info_fpath);
        strcat(info_fpath, "/comm");

        FILE *info = fopen(info_fpath, "r");
        
        if (info)
        {
            if (stat(user_info_fpath, &user_info) == -1)
            {
                fprintf(stderr, "ps: error %s: %s\n", user_info_fpath, strerror(errno));
                return;
            } 
            struct passwd *pw = getpwuid(user_info.st_uid);
            if (!pw)
            {
                fprintf(stderr, "ps error: %s\n", strerror(errno));
                return;
            }
            char *cmdline = NULL;
            size_t _ = 0;
            if (!getline(&cmdline, &_, info))
            {
                fprintf(stderr, "ps: Cannot access %s\n", info_fpath);
            }
            printf("%s%.*s%s%.*s%s", pw->pw_name, USER_WIDTH-(int)strlen(pw->pw_name), LONGSPACESTR,
                                     dent->d_name, PID_WIDTH-(int)strlen(dent->d_name), LONGSPACESTR,
                                     cmdline);
            free(cmdline);
            fclose(info);
        }

        dent = readdir(pd);
    }
    closedir(pd);
}
