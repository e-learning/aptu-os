#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <sys/param.h>
#include <dlfcn.h>

using namespace std;

bool IsNumeric(const char *str)
{
    for (; *str; str++)
        if (*str < '0' || *str > '9')
            return 0;
    return 1;
}

void help()
{
    std::cout << "Available commands:" << std::endl;
    std::cout << "ls" << std::endl;
    std::cout << "pwd" << std::endl;
    std::cout << "ps" << std::endl;
    std::cout << "kill <SIG> <PID>" << std::endl;
    std::cout << "exit" << std::endl << std::endl;
}

int child(const char *name, char **argv)
{
    return execvp(name, argv);
}

void free_memory(char **argv, int argc)
{
        int i = 0;
        for (i = 0; i <= argc; i++)
                free(argv[i]);
        free(argv);

}

int argv_size(char *arguments, int size)
{
        int counter = 0;
        int i = 0;

        if (arguments != NULL)
            counter = 1;

        for (i = 0; i < size; i++)
        {
                if (arguments[i] == ' ')
                        counter++;
        }

        return counter;
}

char **copy_args(int argc, char *command_name)
{
        int i;
        char *substring;
        const char delimiters[] = " ";

        char **args = (char **)malloc(sizeof(char *) * (argc + 1));
        if (!args)
                return (char **)NULL;
        substring = strtok(command_name, delimiters);
        args[0] = strdup(substring);

        for (i = 1; i < argc; i++)
        {
                substring = strtok(NULL, delimiters);
                args[i] = strdup(substring);
        }

        args[argc] = (char *)NULL;
        return args;
}

int exec_pwd_command(char **args, int argc)
{
    (void)args;
    if (argc != 1)
        cout << "our pwd doesn't have any parameters" << endl;
    else
        cout << get_current_dir_name() << endl;
    return 0;
}

int exec_ps_command(char **args, int argc)
{
    (void)args;
    if (argc != 1)
        cout << "our ps doesn't have any parameters" << endl;
    else
    {
        DIR *dir;

        string procdir = "/proc/";

        if ((dir = opendir(procdir.c_str())) == 0)
            cout << "Couldn't read proc directory" << endl;
        else {
            dirent *pid;
            string commandLine;
            while ((pid = readdir(dir)) != 0) {
                if (pid->d_type == DT_DIR && IsNumeric(pid->d_name)) {
                    string cmdFilePath = procdir + string(pid->d_name) + "/comm";
                    ifstream cmdFile(cmdFilePath.c_str());

                    if (cmdFile.is_open()) {
                        getline(cmdFile, commandLine);
                        cout << pid->d_name << ": " << commandLine << endl;
                    }
                    cmdFile.close();
                }
            }
            closedir(dir);
        }
    }
    return 0;
}

int exec_kill_command(char **args, int argc)
{
    if (argc != 3 || !IsNumeric(args[1]) || !IsNumeric(args[2]))
        std::cout << "Our kill has 2 integer parameters" << std::endl;
    else
    {
        int signal = atoi(args[0]);
        int pid = atoi(args[1]);
        kill(pid, signal);
    }
    return 0;
}

int exec_ls_command(char **args, int argc)
{
    (void)args;
    if (argc != 1)
        cout << "our ls doesn't have any parameters" << endl;
    else
    {
        DIR *dir;
        std::string cur = get_current_dir_name();

        if ((dir = opendir(cur.c_str())) == 0)
            cout << "Couldn't read current directory: " << cur << endl;
        else {
            dirent *file;
            while ((file = readdir(dir)) != 0) {
                if (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
                    cout << file->d_name << endl;
            }
            closedir(dir);
        }
    }
    return 0;
}

void handler(int signum)
{
    (void)signum;
    cout << "Dont press Ctrl+C please" << endl;
    exit(-1);
}

int execPipe(string input);
int exec(string input)
{
    pid_t pid;
    int status;
    char arguments[256];
    char name[256];
    char *cmd;
    while (input[0] == ' ')
        input.erase(input.begin(), input.begin() + 1);
    if (input.size() == 0)
        return 0;
    strcpy(name, input.c_str());
    cmd = strtok(name, " ");
    strncpy(arguments, input.c_str(), strlen(input.c_str()) + 1);
    int argc = argv_size(arguments, strlen(arguments));
    char **args = copy_args(argc, arguments);
    if (!strcmp(cmd, "pwd"))
    {
        exec_pwd_command(args, argc);
        free_memory(args, argc);
        return 0;
    }
    if (!strcmp(cmd, "exit"))
    {
        free_memory(args, argc);
        exit(0);
    }
    if (!strcmp(cmd, "ls"))
    {
        exec_ls_command(args, argc);
        free_memory(args, argc);
        return 0;
    }
    if (!strcmp(cmd, "ps"))
    {
        exec_ps_command(args, argc);
        free_memory(args, argc);
        return 0;
    }
    if (!strcmp(cmd, "kill"))
    {
        exec_kill_command(args, argc);
        free_memory(args, argc);
        return 0;
    }
    pid = fork();
    if (pid < 0)
    {
        perror("cannot create process");
        return -1;
    }
    if ((pid == 0) && child(cmd, args))

    {
        perror("cannot execute child process");
        return -1;
    }
    waitpid(pid, &status, 0);
    free_memory(args, argc);
    return 0;
}

int eval(string input)
{
    exec(input);
    return 0;
}
/*
int execPipe(string input)
{
    const int PIPE_READ = 0;
    const int PIPE_WRITE = 1;
    int fileDescriptor[2] = {0, 0};
    int pipe_pos = input.rfind('|');
    string firstCommand = input.substr(0, pipe_pos);
    string secondCommand = input.substr(pipe_pos + 1);

    cout << pipe(fileDescriptor);
    pid_t pid = fork();
    if (pid == 0) {
        dup2(fileDescriptor[1], 1);
        dup2(fileDescriptor[0], 0);
//        close(fileDescriptor[PIPE_READ]);
//        close(fileDescriptor[PIPE_WRITE]);
        eval(firstCommand);
        exit(0);
    } else {
        // parent
//        close(fileDescriptor[PIPE_WRITE]);
//        close(fileDescriptor[PIPE_READ]);
        int status = 0;
        waitpid(pid, &status, 0);
        eval(secondCommand);
    }
    return 0;
}
*/
int main()
{
    struct sigaction sig;

    string inputCommand;

    sig.sa_handler = handler;
    sigaction(SIGINT, &sig, NULL);
    do
    {
        cout << "> ";
        getline(cin, inputCommand);
        if (inputCommand.size() == 0)
            continue;
/*        if (inputCommand.find('|') != string::npos)
        {
            execPipe(inputCommand);
            continue;
        }*/
        else
            exec(inputCommand);
    }
    while (1);
    return 0;
}
