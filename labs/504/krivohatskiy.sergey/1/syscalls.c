#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

void my_kill(int pid, int sig)
{
    if(kill(pid, sig))
    {
        printf("%s\n", "Kill failed");
    }
}

void my_get_current_dir_name()
{
    char buff[512];
    if(getcwd(buff, 512))
    {
        printf("%s\n", buff);
    }
}

void print_dir_name(int *remained, int console_widht, char *dir_name)
{
    int d_name_len = strlen(dir_name);
    if(*remained >= d_name_len + 1)
    {
        printf("%s ", dir_name);
        *remained -= d_name_len + 1;
        return;
    }
    if(d_name_len >= console_widht)
    {
        if(*remained != console_widht)
        {
            printf("\n");
        }
        printf("%s\n", dir_name);
        *remained = console_widht;
        return;
    }
    printf("\n%s ", dir_name);
    *remained = console_widht - d_name_len - 1;
}

void my_read_dir(char* dirname)
{

    DIR *dp;
    struct dirent *ep;
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int console_widht = w.ws_col;
    int remained = console_widht;
    dp = opendir (dirname);
    if (dp != NULL)
    {
        while ((ep = readdir (dp)))
        {
            print_dir_name(&remained, console_widht, ep->d_name);
        }
        closedir(dp);
        printf("\n");
    }
    else
    {
        printf("%s\n", "Opendir failed");
    }
}

char** copy_args(int argc, char **argv)
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

int child(char *name, char ** argv)
{
    return execvp(name, argv);
}

void my_execute(char** argv, int argc)
{
    char *name = argv[0];
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

typedef struct
{
    int pid;
    char comm[256];
} stat_stuff;

static int read_stat(int pid, stat_stuff *s) {

    const char *format = "%d %su";

    char buf[256];
    FILE *proc;
    sprintf(buf,"/proc/%d/stat",pid);
    proc = fopen(buf,"r");
    if (proc) {
        if (2==fscanf(proc, format, &s->pid, s->comm))
        {
            fclose(proc);
            return 1;
        }
        else
        {
            fclose(proc);
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

static void print_stat(stat_stuff *stuff) {
    printf("pid = %d ", stuff->pid);
    printf("comm = %s\n", stuff->comm);
}

void my_ps()
{
    stat_stuff stat;
    struct dirent *ep;
    DIR *dp = opendir("/proc");
    char *unused;
    if (dp != NULL)
    {
        while ((ep = readdir (dp)))
        {
            int pid = strtol(ep->d_name, &unused, 10);
            if(pid && read_stat(pid, &stat))
            {
                print_stat(&stat);
            }
        }
        closedir(dp);
    }
    else
    {
        printf("%s\n", "Opendir failed");
    }

}
