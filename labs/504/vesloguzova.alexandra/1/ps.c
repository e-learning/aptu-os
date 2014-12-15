#include "common.h"

void ps_run()
{
    DIR *pd = opendir("/proc");
    if (!pd)
    {
        perror("opendir");
        exit(1);
    }

    struct dirent *dent = readdir(pd);
    while (dent != 0)
    {
        if (strcmp(dent->d_name, "self") == 0)
        {
            dent = readdir(pd);
            continue;
        }

        char inf_fpath[PATH_MAX];
        strcpy(inf_fpath, "/proc/");
        strcat(inf_fpath, dent->d_name);
        strcat(inf_fpath, "/comm");

        FILE *inf = fopen(inf_fpath, "r");
        if (inf)
        {
            char *cmdline = NULL;
            size_t _ = 0;
            getline(&cmdline, &_, inf);
            printf("%s\t%s", dent->d_name, cmdline);
            free(cmdline);
            fclose(inf);
        }

        dent = readdir(pd);
    }
    closedir(pd);
    return;
}