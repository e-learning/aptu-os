#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <linux/limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>


void handler_ls(const char *args)
{
    const char *path = args ? args : ".";
    DIR *dir = opendir(path);
    if (!dir)
        goto ERROR;

    struct dirent *dent = readdir(dir);
    while (dent != 0) {
        printf("%s\n", dent->d_name);
        dent = readdir(dir);
    }
    closedir(dir);
    return;

ERROR:
    printf("%s\n", strerror(errno));
}

void handler_pwd(const char *_)
{
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    printf("%s\n", path);
}

void handler_ps(const char *_)
{
    DIR *pd = opendir("/proc");
    if (!pd)
        goto ERROR;

    struct dirent *dent = readdir(pd);
    while (dent != 0) {
        char inf_fpath[PATH_MAX];
        strcpy(inf_fpath, "/proc/");
        strcat(inf_fpath, dent->d_name);
        strcat(inf_fpath, "/cmdline");

        FILE *inf = fopen(inf_fpath, "r");
        if (inf) {
            char cmdline[4096];
            fread(cmdline, 1, 4095, inf);
            cmdline[4095] = '\0';
            printf("%s\t%s\n", dent->d_name, cmdline);
            fclose(inf);
        }

        dent = readdir(pd);
    }
    closedir(pd);
    return;

ERROR:
    printf("Can't access /proc\n");
}

void handler_kill(const char *args)
{
    char argstr[256] = {0};
    strncpy(argstr, args, 255);
    long int pid, signal;

    char *p = strtok(argstr, " ");
    if (p == NULL)
        goto NOT_ENOUGH_ARGS;
    errno = 0;
    pid = strtol(p, NULL, 10);
    if (pid < 0 || errno != 0)
        goto ERRONEOUS_ARGS;

    p = strtok(NULL, " ");
    if (p == NULL)
        goto NOT_ENOUGH_ARGS;
    signal = strtol(p, NULL, 10);
    if (signal < 0 || signal > 64 || errno != 0)
        goto ERRONEOUS_ARGS;

    kill(pid, signal);
    return;

ERRONEOUS_ARGS:
    printf("Erroneous arguments\n");
    return;

NOT_ENOUGH_ARGS:
    printf("Usage: kill <pid> <signal>\n");
}

void handler_exit(const char *_)
{
    exit(EXIT_SUCCESS);
}

typedef void (*handler_fn)(const char*);

handler_fn handlers[] =
    { handler_ls
    , handler_pwd
    , handler_ps
    , handler_kill
    , handler_exit
    , NULL
    };

const char *commands[] =
    { "ls"
    , "pwd"
    , "ps"
    , "kill"
    , "exit"
    , NULL
    };


int commands_match(const char *fst, char *snd)
{
    while (*fst != '\0' && *snd != '\0' && *fst == *snd) {
        fst++;
        snd++;
    }
    if (*fst == '\0' && (isspace(*snd) || *snd == '\0'))
        return 1;
    return 0;
}

int main(int argc, const char *argv[])
{
    char cmd[256];
    while (1) {
        printf(">> ");
        fgets(cmd, 255, stdin);
        cmd[strlen(cmd)-1] = '\0';
        const char *args = strchr(cmd, ' ');
        int i = 0;
        while (commands[i]) {
            if (commands_match(commands[i], cmd)) {
                handlers[i](args == NULL ? args : args+1);
                break;
            }
            i++;
        }
        if (!commands[i])
            system(cmd);
    }
    return 0;
}
