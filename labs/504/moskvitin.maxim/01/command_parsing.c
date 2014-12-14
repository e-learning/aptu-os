#include "command_parsing.h"
#include "Vector.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

struct command* create_command()
{
    struct command* cmd = malloc(sizeof(struct command));
    cmd->command_name = NULL;
    cmd->args = VectorCreate(sizeof(void*));
    cmd->input_pipe       = false;
    cmd->output_pipe      = false;
    cmd->input_file       = false;
    cmd->output_file      = false;
    cmd->input_file_name  = NULL;
    cmd->output_file_name = NULL;
    return cmd;
}

void free_command(struct command* cmd)
{
    if (cmd->command_name)
        free(cmd->command_name);
    int i;
    for (i = 0; i < cmd->args->size; ++i)
    {
        free(*(void**)VectorGet(cmd->args, i));
    }
    VectorFree(cmd->args);
    if (cmd->input_file_name)
        free(cmd->input_file_name);
    if (cmd->output_file_name)
        free(cmd->output_file_name);
    free(cmd);
}

void split_args(struct command* cmd)
{
    char* p_command_name_begin = cmd->command_name;
    while (*p_command_name_begin && isspace(*p_command_name_begin))
    {
        ++p_command_name_begin;
    }

    char* p_command_name_end = p_command_name_begin;
    while (*p_command_name_end && !isspace(*p_command_name_end))
    {
        ++p_command_name_end;
    }

    char* p_args = p_command_name_end;
    while (*p_args)
    {
        if (isspace(*p_args))
        {
            ++p_args;
            continue;
        }

        if (*p_args == '"' || *p_args == '\'')
        {
            char quotes = *p_args;
            ++p_args;
            char* begin = p_args;
            while (*p_args && !(*p_args == quotes))
            {
                ++p_args;
            }

            char* arg = malloc(p_args - begin + 1);
            memcpy(arg, begin, p_args - begin);
            arg[p_args - begin] = '\0';

            VectorPush(cmd->args, &arg);

            if (*p_args)
            {
                ++p_args;
            }
        }
        else
        {
            char* begin = p_args;
            while (*p_args && !isspace(*p_args))
            {
                ++p_args;
            }

            char* arg = malloc(p_args - begin + 1);
            memcpy(arg, begin, p_args - begin);
            arg[p_args - begin] = '\0';

            VectorPush(cmd->args, &arg);
        }
    }

    char* command_name = malloc(p_command_name_end - p_command_name_begin + 1);
    memcpy(command_name, p_command_name_begin, p_command_name_end - p_command_name_begin);
    command_name[p_command_name_end - p_command_name_begin] = '\0';

    free(cmd->command_name);
    cmd->command_name = command_name;
}

void extract_redirection_fd(struct command* cmd)
{
    bool ignore_brackets = false;
    char unclosed_quotes = '\0';
    char* p_cmd = cmd->command_name;
    while (*p_cmd)
    {
        if (*p_cmd == '"' || *p_cmd == '\'')
        {
            if (ignore_brackets)
            {
                if (unclosed_quotes == *p_cmd)
                {
                    unclosed_quotes = '\0';
                    ignore_brackets = false;
                }
            }
            else
            {
                unclosed_quotes = *p_cmd;
                ignore_brackets = true;
            }
        }

        if (*p_cmd == '>' || *p_cmd == '<')
        {
            char bracket = *p_cmd;
            char* begin_redirection = p_cmd;
            char* end_redirection;
            ++p_cmd;
            while (isspace(*p_cmd))
                ++p_cmd;

            char* begin_filename;
            char* end_filename;
            if (*p_cmd == '"' || *p_cmd == '\'')
            {
                char quotes = *p_cmd;
                ++p_cmd;

                begin_filename = p_cmd;
                while (*p_cmd && *p_cmd != quotes)
                {
                    ++p_cmd;
                }
                end_filename = p_cmd;

                if (*p_cmd)
                {
                    ++p_cmd;
                }
                end_redirection = p_cmd;
            }
            else
            {
                begin_filename = p_cmd;
                while (*p_cmd && !isspace(*p_cmd))
                {
                    ++p_cmd;
                }
                end_filename = p_cmd;
                end_redirection = p_cmd;
            }

            char* filename = malloc(end_filename - begin_filename + 1);
            memcpy(filename, begin_filename, end_filename - begin_filename);
            filename[end_filename - begin_filename] = '\0';

            char* command_name = malloc(strlen(cmd->command_name) - (end_redirection - begin_redirection) + 1);
            memcpy(command_name, cmd->command_name, begin_redirection - cmd->command_name);
            memcpy(command_name + (begin_redirection - cmd->command_name), end_redirection, strlen(end_redirection) + 1);

            free(cmd->command_name);
            cmd->command_name = command_name;
            p_cmd = cmd->command_name;

            if (bracket == '>')
            {
                cmd->output_file = true;
                if (cmd->output_file_name)
                {
                    free(cmd->output_file_name);
                }
                cmd->output_file_name = filename;
                cmd->output_pipe = false;
            }
            else
            {
                cmd->input_file = true;
                if (cmd->input_file_name)
                {
                    free(cmd->input_file_name);
                }
                cmd->input_file_name = filename;
                cmd->input_pipe = false;
            }
        }
        else
        {
            ++p_cmd;
        }
    }
}

struct Vector* split_by_pipeline(char* command)
{
    bool ignore_pipeline = false;
    char unclosed_quotes = '\0';
    char* p_command = command;
    char* last_begin = command;
    struct Vector* commands = VectorCreate(sizeof(void*));
    while (*p_command)
    {
        if (*p_command == '"' || *p_command == '\'')
        {
            if (ignore_pipeline)
            {
                if (unclosed_quotes == *p_command)
                {
                    unclosed_quotes = '\0';
                    ignore_pipeline = false;
                }
            }
            else
            {
                unclosed_quotes = *p_command;
                ignore_pipeline = true;
            }
        }

        if (!ignore_pipeline && *p_command == '|')
        {
            struct command* single_command = create_command();

            single_command->command_name = malloc(p_command - last_begin + 1);
            memcpy(single_command->command_name, last_begin, p_command - last_begin);
            single_command->command_name[p_command - last_begin] = '\0';

            if (commands->size)
                single_command->input_pipe = true;
            single_command->output_pipe = true;

            VectorPush(commands, &single_command);
            last_begin = p_command + 1;
        }
        ++p_command;
    }

    if (last_begin != p_command)
    {
        struct command* single_command = create_command();

        single_command->command_name = malloc(p_command - last_begin + 1);
        memcpy(single_command->command_name, last_begin, p_command - last_begin);
        single_command->command_name[p_command - last_begin] = '\0';

        if (commands->size)
            single_command->input_pipe = true;

        VectorPush(commands, &single_command);
    }

    return commands;
}

struct Vector* parse_command(char* command)
{
    struct Vector* commands = split_by_pipeline(command);
    int i = 0;
    for (; i < commands->size; ++i)
    {
        extract_redirection_fd(*(struct command**)VectorGet(commands, i));
        split_args(*(struct command**)VectorGet(commands, i));
    }
    return commands;
}
