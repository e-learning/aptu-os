#include <stdio.h> // stderr
#include <sys/types.h> // opendir, closedir, scandir
#include <dirent.h> // struct dirent
#include <stdlib.h> // free
#include <string.h> // strlen
#include <unistd.h> // getcwd 
#include <pwd.h>
#include <sys/stat.h>
#include <errno.h>
#include "libps.h"

#define USER_WIDTH 20
#define PID_WIDTH 10
#define LONGSPACESTR "                                          "

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
