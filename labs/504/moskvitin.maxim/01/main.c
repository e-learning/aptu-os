#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include "ls.h"
#include "ps.h"

void parseAndExecCommand(const char* command);
void execUnknownCommand(const char* command, char * const args[]);
char** parseExtern(const char* str, size_t* count);
void pwd();
int kill_proc(char * const args[]);
void freeArg(char **args);

void signal_handler(int signal)
{
    printf("\nInterrapted from keyboard\n");
    exit(signal);
}

int main(int argc, char** argv)
{
    struct sigaction sig_action;
    sig_action.sa_handler = signal_handler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;

    sigaction(SIGINT, &sig_action, NULL);

    size_t commandLen = 0;
    char* command;
    while (1)
    {
        getline(&command, &commandLen, stdin);
        command[strlen(command) - 1] = '\0';
        parseAndExecCommand(command);
        free(command);
        commandLen = 0;
    }
    return 0;
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

void parseAndExecCommand(const char* command)
{
    size_t argC = 0;
    char** commandArgs = parseExtern(command, &argC);
    if (argC == 0)
        return;
    if (!strcmp(commandArgs[0], "exit"))
        exit(0);
    else if (!strcmp(commandArgs[0], "ls"))
        ls();
    else if (!strcmp(commandArgs[0], "pwd"))
        pwd();
    else if (!strcmp(commandArgs[0], "ps"))
        ps();
    else if (!strcmp(commandArgs[0], "kill"))
        kill_proc(commandArgs + 1);
    else
        execUnknownCommand(commandArgs[0], commandArgs);
    freeArg(commandArgs);
}

void freeArg(char **argV)
{
    char** p_arg = argV;
    while (*p_arg)
        free((void *)*(p_arg++));
    free((void *)argV);
}

void execUnknownCommand(const char* name, char * const args[])
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Cannot create process");
    }
    else if (pid == 0 && execvp(name, args))
    {
        perror("Error on execute child process");
        exit(-1);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

char** parseExtern(const char* str, size_t* count)
{
    size_t len = strlen(str);
    *count = 0;
    size_t i;

    if (len == 0)
        return NULL;

    for (i = 0; i < len - 1; ++i)
    {
        if (str[i] == ' ')
        {
            ++(*count);
            while (++i < len && str[i] == ' ');
        }
    }
    if (len > 0 && str[len - 1] != ' ')
        ++(*count);

    char** result = malloc(sizeof(char*) * (*count + 1));
    for (i = 0; i < *count; ++i)
    {
        const char* begin = str;
        const char* end = str;
        while (*end != ' ' && *end != '\0' && *end != '\n')
            ++end;
        result[i] = malloc(sizeof(char) * (end - begin + 1));
        memcpy(result[i], begin, sizeof(char) * (end - begin));
        result[i][end - begin] = '\0';
        str = end;
        while (*str == ' ')
            ++str;
    }
    result[*count] = (char*)NULL;
    return result;
}
