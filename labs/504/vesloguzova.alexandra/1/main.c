#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h> // wait, waitpid


size_t command_size = MAX_INPUT;

void ls_run();

void ps_run();

void pwd_run();

void kill_run(char *command);

void execute(char **args);

char **parse_args(char *str, size_t *arg_number);

int main(void)
{
    char *command_name;
    const char  ls_command[]    = "ls",
                ps_command[]    = "ps",
                pwd_command[]   = "pwd",
                kill_command[]  = "kill",
                exit_command[]  = "exit";


    command_name = (char *) malloc(command_size + 1);
    while (1)
    {
        long read_bytes;
        read_bytes = getline(&command_name, &command_size, stdin);
        if (read_bytes == -1)
            exit(1);
        else if (strncmp(ls_command, command_name, 2) == 0)
            ls_run();
        else if (strncmp(ps_command, command_name, 2) == 0)
            ps_run();
        else if (strncmp(pwd_command, command_name, 3) == 0)
            pwd_run();
        else if (strncmp(kill_command, command_name, 4) == 0)
            kill_run(command_name);
        else if (strncmp(exit_command, command_name, 4) == 0)
        {
            printf("Terminated by user command\n");
            break;
        }
        else
        {

            size_t args_number;
            char **args = parse_args(command_name, &args_number);
            execute(args);
        }
    }
    free(command_name);

    return 0;
}

void ls_run()
{
    DIR *dir = opendir("./");
    if (!dir)
    {
        perror("opendir");
        exit(1);
    }
    struct dirent *dent = readdir(dir);
    while (dent != 0)
    {
        printf("%s\n", dent->d_name);
        dent = readdir(dir);
    }
    closedir(dir);
    return;

}

void ps_run()
{
    DIR *pd = opendir("/proc");
    if (!pd)
    {
        perror("opendir");
        exit(1);
    }

    struct dirent *dent = readdir(pd);
    while (dent != 0)
    {
        if (strcmp(dent->d_name, "self") == 0)
        {
            dent = readdir(pd);
            continue;
        }

        char inf_fpath[PATH_MAX];
        strcpy(inf_fpath, "/proc/");
        strcat(inf_fpath, dent->d_name);
        strcat(inf_fpath, "/comm");

        FILE *inf = fopen(inf_fpath, "r");
        if (inf)
        {
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
}

void pwd_run()
{
    char path[PATH_MAX];
    if (getcwd(path, PATH_MAX) == NULL)
    {
        perror("getcwd");
        exit(1);
    }
    printf("%s\n", path);
}

int parse_pid(char *command)
{
    size_t i, flag = 0, start = 0;

    for (i = 4; i < command_size; i++)
    {
        if (command[i] == ' ')
        {
            flag = 1;
            start = i;
        } else
        {
            if (command[i] == '\0') break;
            else
            {
                if (flag && command[i] != ' ')
                {
                    break;
                }
            }
        }
    }
    if ((flag == 1 && command[i] == '\0') || (flag == 0 && start == i)) return -1;//no pid

    return atoi(command + start);
}

void kill_run(char *command)
{
    int pid = 0;

    pid = parse_pid(command);

    if (kill(pid, SIGTERM) == 0)
        printf("Process with PID: %d succesfully killed\n", pid);
    else
        printf("Cannot kill process with PID: %d or it doesn't exist\n", pid);
}

int child(const char *name, char *const *argv)
{
    printf("%d, starting process %s...\n", getpid(), name);
    return execvp(name, argv);
}

void execute(char **args)
{

    pid_t pid = fork();
    int status;
    if (pid < 0)
    {
        perror("cannot create process");
    }

    if (pid == 0)
    {
        if (child(args[0], args) == -1)
        {
            perror("cannot execute child process");
        }
        exit(EXIT_SUCCESS);

    }

    waitpid(pid, &status, 0);
}

char **parse_args(char *str, size_t *arg_number)
{
    char **result;
    size_t argsnum = 0;

    size_t i = 0;
    char ch;
    while (str[i] == ' ' && str[i] != '\0')
        ++i;
    if (str[i] != '\0')
        ++argsnum;
    while (str[i] != '\0')
    {
        ch = str[i++];
        if (ch == '\"')
        {
            while (str[i++] != '\"');
        } else if (ch == '\'')
        {
            while (str[i++] != '\'');
        } else if (ch == ' ')
        {
            if (str[i - 2] != ' ')
            {
                ++argsnum;
            }
        } else if (ch == '\n')
        {
            str[--i] = '\0';
            break;
        }
    }
    if (str[i - 1] == ' ')
        --argsnum;

    result = (char **) malloc(sizeof(char *) * (argsnum + 1));
    result[argsnum] = NULL;

    size_t arg = 0, argindex = 0;
    size_t currarglen;
    i = 0;
    while (str[i] == ' ' && str[i] != '\0')
        ++i;
    while (str[i] != '\0')
    {
        ch = str[i];
        argindex = 0;
        currarglen = 0;

        switch (ch)
        {
            case '\"':
            case'\'':
                ++i;
                while (str[i] != ch && str[i] != '\0')
                {
                    if (str[i + 1] != '\0') break;
                    ++i;
                    ++currarglen;
                }

                result[arg] = (char *) malloc(currarglen);
                i -= currarglen;
                result[arg][argindex++] = ch;
                while (str[i] != '\0' && str[i] != ch)
                    result[arg][argindex++] = str[i++];
                if (str[i] != '\0')
                    result[arg][argindex++] = str[i++];
                result[arg++][argindex] = '\0';
                break;
            case ' ':
                while (str[i] == ' ')
                    ++i;
                break;
            default:
                if (arg < argsnum)
                {
                    while (str[i] != ' ' && str[i] != '\0')
                    {
                        ++i;
                        ++currarglen;
                    }
                    result[arg] = (char *) malloc(currarglen);
                    i -= currarglen;
                    while (str[i] != ' ' && str[i] != '\0')
                    {
                        result[arg][argindex++] = str[i++];
                    }
                    result[arg++][argindex] = '\0';
                    break;
                } else
                {
                    ++i;
                }

        }
    }

    *arg_number = argsnum;


    return result;
}
