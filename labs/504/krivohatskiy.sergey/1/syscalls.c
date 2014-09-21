#include <unistd.h>
#include "syscalls.h"
#include "stdio.h"
#include <sys/types.h>
#include <syscall.h>
#include <signal.h>
#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>

void my_kill(int pid, int sig)
{
    if(kill(pid, sig))
    {
        // TODO: process error
    }
}

void my_get_current_dir_name()
{
    char* cwd = get_current_dir_name();
    printf("%s\n", cwd);
    free(cwd);
}

void my_read_dir(char* dirname)
{

    DIR *dp;
    struct dirent *ep;

    dp = opendir (dirname);
    if (dp != NULL)
      {
        while ((ep = readdir (dp)))
          printf("%s\n", ep->d_name);
        (void) closedir (dp);
      }
    else
      puts ("Couldn't open the directory.");
}

char** copy_args(int argc, const char **argv)
{
    int i;
    int size = sizeof(char*) * (argc + 1);
    char **args = malloc(size);

    for (i = 0; i < argc; ++i)
        args[i] = strdup(argv[i]);
    args[argc] = (char* )NULL;

    return args;
}

void free_args(char** argv)
{
    char** pargv = argv;
    while (*pargv)
        free((void*)*pargv++);
    free((void*)argv);
}

int child(const char *name, char * const * argv)
{
    return execvp(name, argv);
}

void my_execute(char** argv, int argc)
{
    const char *name = argv[0];
    pid_t pid;
    int status;

    char **args = copy_args(argc + 1, argv);
    if (!args) {
        perror("cannot allocate memory");
        return;
    }

    pid = fork();
    if (pid < 0) {
        perror("cannot create process");
        free_args(args);
        return;
    }

    if ((pid == 0) && child(name, args)) {
        perror("cannot execute child process");
        free_args(args);
        exit(0);
    }

    waitpid(pid, &status, 0);
    free_args(args);
}
