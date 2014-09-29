#include "command_executing.h"
#include <strings.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "syscalls.h"


#define COMMAND_NAMES_COUNT 5

char* COMMAND_NAMES[COMMAND_NAMES_COUNT] = {"LS", "PWD", "PS", "KILL", "EXIT"};

typedef enum
{
    LS = 0, PWD = 1, PS = 2, KILL = 3, EXIT = 4, EXECUTE
} command_ids;

typedef struct
{
    command_ids id;
    int args_count;
    char** args;
} command;


void parse_command_string(char* command_string, command* cmd)
{

    char* first_space = index(command_string, ' ');
    if(first_space == NULL)
    {
        cmd->args = malloc(sizeof(char*) * (1));
        cmd->args[0] = command_string;
        cmd->args_count=0;
        return;
    }
    *first_space = 0;
    char* args = first_space + 1;
    int spaces_count = 0;
    char* s;
    for (s = args; s[spaces_count];)
    {
        if(s[spaces_count] == ' ')
        {
            spaces_count++;
        }
        else
        {
            s+=1;
        }
    }
    cmd->args_count = spaces_count + 1;
    cmd->args = malloc(sizeof(char*) * (cmd->args_count + 1));
    cmd->args[0] = command_string;
    cmd->args[1] = args;
    spaces_count = 0;
    for (s = args; s[spaces_count];)
    {
        if(s[spaces_count] == ' ')
        {
            cmd->args[spaces_count + 2] = s + spaces_count + 1;
            s[spaces_count] = 0;
            spaces_count++;
        }
        else
        {
            s+=1;
        }
    }
}
void parse_command_name(char* command_string, command* cmd)
{
    int i;
    for(i =0; i < COMMAND_NAMES_COUNT; i++)
    {
        if(!strcasecmp(command_string, COMMAND_NAMES[i]))
        {
            cmd->id = i;
            return;
        }
    }
    cmd->id = EXECUTE;
}

command parse_command(char* command_string)
{
    command cmd;
    parse_command_string(command_string, &cmd);
    parse_command_name(command_string, &cmd);
    return cmd;
}

bool execute_command(char* command_string)
{
    command cmd = parse_command(command_string);
    switch (cmd.id) {
    case EXECUTE:
        my_execute(cmd.args, cmd.args_count);
        break;
    case LS:
        if(cmd.args_count == 0)
        {
            my_read_dir(".");
        } else {
            my_read_dir(cmd.args[1]);
        }
        break;
    case PWD:
        my_get_current_dir_name();
        break;
    case PS:
        my_ps();
        break;
    case KILL:
        if(cmd.args_count < 2)
        {
            printf("usage: kill pid sig");
        } else {
            my_kill(atoi(cmd.args[1]), atoi(cmd.args[2]));
        }
        break;
    case EXIT:
        // Do nothing
        break;
    }
    free(cmd.args);
    return cmd.id != EXIT;
}
