#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "dirent.h"
#include "string.h"
#include <fcntl.h>
#include <signal.h>
#include "wait.h"

#define BUFFER_SIZE 100
#define MAX_PATH 100

void ls_command(int argc, char ** argv)
{
    DIR * dir;
    struct dirent * entry;
    dir = opendir(".");
    if (dir == NULL)
    {
        perror("Cannot to open directory\n");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if ((strcmp(entry->d_name, ".")) != 0 && (strcmp(entry->d_name, "..") != 0))
        {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void pwd_command(int argc, char ** argv)
{
    char * cur_dir = getcwd(NULL, 0);
    if (cur_dir == NULL)
    {
        perror("Cannot read directory");
    }
    else
    {
        printf("%s\n", cur_dir);
    }
}

void ps_command(int argc, char ** argv)
{
    DIR * dir;
    struct dirent * entry;
    if ((dir = opendir("/proc")) == NULL)
    {
        perror("Unable to open /proc");
    }
    printf("%.12s%25s\n", "PPID", "COMM");
    char * proc_path = (char*) malloc(MAX_PATH);
    while((entry = readdir(dir)) != NULL)
        if (entry->d_type == DT_DIR && is_num(entry->d_name)) {

            sprintf(proc_path, "%s/%s/comm", "/proc",entry->d_name);
            int comm;
            comm = open(proc_path, O_RDONLY);
            if (comm < 0) {
                perror("Unable to open comm");
                return;
            };
            char buffer[BUFFER_SIZE + 1];
            ssize_t read_bytes;

            while ((read_bytes = read(comm, buffer, BUFFER_SIZE)) > 0) {
                buffer[read_bytes] = 0;
                printf("%.12s%25s", entry->d_name,buffer);
            }
            close(comm);
        }
    free(proc_path);
}

void kill_command(int argc, char ** argv)
{
    pid_t pid;
    unsigned signal;
    printf("signal %s, pid %s\n", argv[0], argv[1]);
    if (argc != 2)
    {
        printf("Wrong arguments. Use kill <signal> <pid>\n");
	printf("Args count %d", argc);
        return;
    }
    if ((sscanf(argv[1],"%u", &pid)) && (sscanf(argv[0], "%u", &signal)))
    {
        if (kill(pid, signal))
        {
            printf("Kill error.\n");
        }
    }
    else
    {
        printf("Wrong arguments. Use kill <signal> <pid>\n");
    }
    
}

void exec_command(int argc, char ** argv)
{
    pid_t pid = fork();
    int status = 0;
    if (pid < 0)
    {
        perror("Fork error.\n");
        return;
    }
    if (pid == 0)
    {
        if (execvp(argv[0], argv))
        {
            perror("Exec error.\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        waitpid(pid, &status, 0);
    }
}

void exit_command(int argc, char **argv)
{
    exit(EXIT_SUCCESS);
}
