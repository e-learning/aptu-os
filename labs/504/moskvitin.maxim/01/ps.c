#include "ps.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* getPathToStat(char* pid)
{
    char *path = malloc(sizeof(char) * 255);
    path[0] = '\0';
    strcat(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/stat");
    return path;
}

int ps()
{
    DIR* proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        perror("Cannot read process list");
        return -1;
    }

    char id[8];
    char command[255];
    struct dirent* proc_entry;
    do
    {
        proc_entry = readdir(proc_dir);
        if (proc_entry != NULL && proc_entry->d_type == DT_DIR)
        {
            int pid = atoi(proc_entry->d_name);
            if (pid != 0)
            {
                char* path = getPathToStat(proc_entry->d_name);
                if (path == NULL)
                {
                    perror("Cannot allocate memory");
                    closedir(proc_dir);
                    return -1;
                }
                FILE* process = fopen(path, "r");
                free(path);
                if (process == NULL)
                {
                    perror("Cannot read process description");
                    continue;
                }
                fscanf(process, "%s %s", id, command);
                fclose(process);
                printf("%s\t%s\n", id, command);
            }
        }
    } while(proc_entry != NULL);

    closedir(proc_dir);
    return 0;
}

