#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "sh.h"


unsigned token_count = 0;


int tokenize(char * string)
{
    char command[TOKEN_SIZE];
    char * token;
    size_t token_len;
    strcpy(command, string);
    token = strtok(command, " \t\n");
    while(token)
    {
        if ((token_len = strlen(token)) > 0)
        {
            tokens[token_count] = (char *) malloc(token_len);
            strcpy(tokens[token_count++], token);
        }
        token = strtok(NULL, " \t\n");
    }
    tokens[token_count] = NULL;
    return token_count;
}

void free_tokens()
{
    for (;token_count != 0;)
    {
        free((void *)tokens[--token_count]);
    }
}

void run_command(int argc, char ** argv)
{
    if (strcmp(argv[0], "ls") == 0)
    {
        ls_command(argc - 1, argv + 1);
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        pwd_command(argc - 1, argv + 1);
    }
    else if (strcmp(argv[0], "ps") == 0)
    {
        ps_command(argc - 1, argv + 1);
    }
    else if (strcmp(argv[0], "kill") == 0)
    {
        kill_command(argc - 1, argv + 1);
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
        exit_command(argc - 1, argv + 1);
    }
    else
    {
         exec_command(argc, argv);
    }

}

void run()
{
    char command[MAX_COMMAND_LENGTH];
    while(1)
    {
        printf(":>");
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        tokenize(command);
        if (token_count == 0)
        {
            continue;
        }
        run_command(token_count, tokens);
        free_tokens();
    }
}



void signal_handler(int sig)
{
    switch (sig)
    {
        case SIGINT:
	    printf("SIGINT received\n");
            exit(EXIT_SUCCESS);
        default:
            break;
    }
}


