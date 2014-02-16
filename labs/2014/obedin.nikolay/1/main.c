#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <linux/limits.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>


void handler_ls(const char *args)
{
    const char *path = args ? args : ".";
    DIR *dir = opendir(path);
    if (!dir)
        goto OPEN_ERROR;

    struct dirent *dent = readdir(dir);
    while (dent != 0) {
        printf("%s\n", dent->d_name);
        dent = readdir(dir);
    }
    closedir(dir);
    return;

OPEN_ERROR:
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
        goto OPEN_ERROR;

    struct dirent *dent = readdir(pd);
    while (dent != 0) {
        if (strcmp(dent->d_name, "self") == 0) {
            dent = readdir(pd);
            continue;
        }

        char inf_fpath[PATH_MAX];
        strcpy(inf_fpath, "/proc/");
        strcat(inf_fpath, dent->d_name);
        strcat(inf_fpath, "/comm");

        FILE *inf = fopen(inf_fpath, "r");
        if (inf) {
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

OPEN_ERROR:
    printf("Can't access /proc\n");
}

void handler_kill(const char *args)
{
    if (args == NULL)
        goto NOT_ENOUGH_ARGS;

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

    if (kill(pid, signal) == -1)
        goto ERROR_WHILE_KILLING;
    return;

ERROR_WHILE_KILLING:
    printf("%s\n", strerror(errno));
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

const char *find_args_start(const char *cmd)
{
    const char *start = strpbrk(cmd, " \t");
    while (start && isspace(*start))
        start++;
    return start && *start != '\0' ? start : NULL;
}

int main(int argc, const char *argv[])
{
    while (1) {
        printf(">> ");
        char *cmd  = NULL;
        size_t _ = 0;
        ssize_t len = getline(&cmd, &_, stdin);
        if (len <= 0)
            continue;
        cmd[len-1] = '\0';

        int i = 0;
        while (commands[i]) {
            if (commands_match(commands[i], cmd)) {
                const char *args = find_args_start(cmd);
                handlers[i](args);
                break;
            }
            i++;
        }

        if (!commands[i])
            system(cmd);
        free(cmd);
    }
    return 0;
}
