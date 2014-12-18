#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>

#include "Vector.h"
#include "command_parsing.h"
#include "command_execute.h"

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
        Vector* commands = parse_command(command);
        exec_commands(commands);
        size_t i;
        for (i = 0; i < commands->size; ++i)
        {
            struct command* cmd = *(struct command**)VectorGet(commands, i);
            free_command(cmd);
        }
        VectorFree(commands);
        free(command);
        commandLen = 0;
    }
    return 0;
}






