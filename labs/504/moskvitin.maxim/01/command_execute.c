#include "command_execute.h"
#include "command_parsing.h"
#include "ls.h"
#include "ps.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void pwd();
int kill_proc(char * const args[]);

void freeArg(char **argV)
{
    char** p_arg = argV;
    while (*p_arg)
        free((void *)*(p_arg++));
    free((void *)argV);
}

pid_t exec_command(struct command* cmd, int *prev_pipefd, int *next_pipefd)
{
    pipe(next_pipefd);
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Cannot create process");
    }
    else if (pid == 0)
    {
        close(prev_pipefd[1]);
        if (cmd->input_pipe)
        {
            dup2(prev_pipefd[0], STDIN_FILENO);
        }
        close(prev_pipefd[0]);

        if (cmd->input_file)
        {
            if (!freopen(cmd->input_file_name, "r", stdin))
            {
                printf("File not exists %s\n", cmd->input_file_name);
                exit(1);
            }
        }

        close(next_pipefd[0]);
        if (cmd->output_pipe)
        {
            dup2(next_pipefd[1], STDOUT_FILENO);
        }
        close(next_pipefd[1]);

        if (cmd->output_file)
        {
            freopen(cmd->output_file_name, "w", stdout);
        }

        if (!strcmp(cmd->command_name, "exit"))
            exit(0);
        else if (!strcmp(cmd->command_name, "ls"))
            ls();
        else if (!strcmp(cmd->command_name, "pwd"))
            pwd();
        else if (!strcmp(cmd->command_name, "ps"))
            ps();
        else if (!strcmp(cmd->command_name, "kill"))
            kill_proc((char**)cmd->args->elems);
        else if (!strcmp(cmd->command_name, ""))
            exit(0);
        else
        {
            Vector* args = VectorCreate(sizeof(void*));
            VectorPush(args, &cmd->command_name);
            size_t i;
            for (i = 0; i < cmd->args->size; ++i)
            {
                VectorPush(args, (void**)VectorGet(cmd->args, i));
            }
            if (execvp(cmd->command_name, (char**)args->elems))
            {
                perror("Error on execute child process");
                exit(-1);
            }
        }
        exit(0);
    }
    else
    {
        close(prev_pipefd[0]);
        close(prev_pipefd[1]);
        if (!strcmp(cmd->command_name, "exit"))
            exit(0);
    }
    return pid;
}

void exec_commands(Vector *commands)
{
    pid_t* pids = malloc(sizeof(pid_t) * commands->size);
    int** fds = malloc(sizeof(int*) * (commands->size + 1));
    fds[0] = malloc(sizeof(int) * 2);
    pipe(fds[0]);
    size_t i;
    for (i = 0; i < commands->size; ++i)
    {
        fds[i + 1] = malloc(sizeof(int) * 2);
        struct command* cmd = *(struct command**)VectorGet(commands, i);
        pids[i] = exec_command(cmd, fds[i], fds[i + 1]);
    }

    for (i = 0; i < commands->size; ++i)
    {
        int status;
        waitpid(pids[i], &status, 0);
    }

    free(pids);
    for (i = 0; i <= commands->size; ++i)
    {
        free(fds[i]);
    }
    free(fds);
}

void pwd()
{
    char* wd = getcwd(NULL, 0);
    printf("%s\n", wd);
    free(wd);
}

int kill_proc(char * const args[])
{
    int signal = 0;
    int pid = 0;
    char* const* pargs = args;
    bool isSignal = false;
    while (*pargs)
    {
        if (isSignal)
        {
            signal = atoi(*pargs);
            isSignal = false;
        }
        else if (!strcmp(*pargs, "-s"))
        {
            isSignal = true;
        }
        else
        {
            pid = atoi(*pargs);
        }
        pargs++;
    }

    if (!signal)
    {
        perror("signal not speciefed, -s <signal> is needed");
        return -1;
    }

    if (!pid)
    {
        perror("pid not speciefed or invalid");
        return -1;
    }

    if (kill(pid, signal))
    {
        perror("kill failed");
        return -1;
    }
    return 0;
}
