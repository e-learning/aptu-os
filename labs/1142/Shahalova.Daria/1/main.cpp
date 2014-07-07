#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
using namespace std;

int parse_command(string input_str, string &command, string &args)
{
    // пропускаем возможные пробелы перед командой
    size_t comm_begin = input_str.find_first_not_of(' ', 0);
    if (comm_begin == std::string::npos)
    {
        return -1;
    }
    
    //ищем конец команды
    size_t comm_end = input_str.find_first_of(" ", comm_begin);
    if (comm_end == std::string::npos)
    {
        comm_end = input_str.length();
    }
    
    // извлекаем команду из строки
    command = input_str.substr(comm_begin, comm_end - comm_begin);
    
    // пропускаем возможные пробелы до списка аргументов команды
    size_t args_begin = input_str.find_first_not_of(' ', comm_end);
    if (args_begin == std::string::npos) // нету аргументов
    {
        return 0;
    }
    
    //до конца строки - аргументы
    size_t args_end = input_str.length();
    
    // извлекаем аргументы из строки
    args = input_str.substr(args_begin, args_end - args_begin);
    
    return 0;
}

void print_help()
{
    cout << "************ ИНТЕРПРЕТАТОР КОМАНД ************" << endl;
    cout << "Поддерживаемые команды:" <<endl;
    cout << "   help - выводит данное собщение." << endl;
    cout << endl;
    
    cout << "   exit - выход из интерпретатора." << endl;   
    cout << endl;
    
    cout << "   ls - вывод списка файлов в текущем каталоге." << endl; 
    cout << endl;
    
    cout << "   pwd - вывод полного имени текущего каталога." << endl; 
    cout << endl;
    
    cout << "   ps - вывод списка процессов." << endl; 
    cout << endl;
    
    cout << "   kill - отправка сигнала с заданным номером конкретному процессу." << endl;
    cout << "   синтаксис: kill <сигнал> <PID процесса>." << endl;
    cout << endl;
}

void command_ls()
{
    DIR *cur_dir = opendir(".");
    struct dirent *dirp;
    
    if(cur_dir == NULL) {
        cout << "Ошибка (" << errno << ") при открытии текущего каталога" << endl;
        return;
    }
    
    cout << "Содержимое текущего каталога:" << endl;
    while ((dirp = readdir(cur_dir)) != NULL) {
        cout << "   " << dirp->d_name << endl;
    }
    closedir(cur_dir);
}

void command_pwd()
{
    char cur_dir_name[256];

    if (getcwd(cur_dir_name, sizeof(cur_dir_name)) == NULL)
        cout << "Ошибка в функции getcwd()" << endl;
    else
        cout << "Полное имя текущего каталога: " << cur_dir_name << endl;
}

void command_ps()
{
    DIR *dir_p;
    struct dirent *dir_entry_p;
    char dir_name[256];
    FILE* stat_file;
    int pid;
    char comm[256];

    printf("%6s %51s\n", "PID", "Имя процесса");
    printf("***********************************************\n");
    dir_p = opendir("/proc/");// открываем каталог /proc/
    while((dir_entry_p = readdir(dir_p)) != NULL) {// Reading /proc/ entries
        if(strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) {// проверяем является ли имя каталога числом, т.е. PID процесса 
            strcpy(dir_name, "/proc/");
            strcat(dir_name, dir_entry_p->d_name);
            strcat(dir_name, "/stat");
            stat_file = fopen(dir_name, "rt");
            if(stat_file == NULL)
            {
                cout << "Ошибка при открытии файла статистики для процесса с PID = " << dir_entry_p->d_name << endl;
                return;
            }
            fscanf(stat_file, "%d %s", &pid, &comm);
            printf("%6d %40s\n", pid, comm);
            fclose(stat_file);
        }
    }
    closedir(dir_p);  
}

int parse_kill_args(string args, string &pid, string &sig)
{
    //ищем конец сигнала
    size_t sig_end = args.find_first_of(" ", 0);
    if (sig_end == std::string::npos)
    {
        cout << "kill: нехватает аргументов" << endl;
        return -1;
    }
    
    sig = args.substr(0, sig_end);
    
    // пропускаем возможные пробелы до списка PID процесса
    size_t pid_begin = args.find_first_not_of(' ', sig_end);
    if (pid_begin == std::string::npos) // нету аргументов
    {
        cout << "kill: нехватает аргументов" << endl;
        return -1;
    }
    
    //до конца строки - PID
    size_t pid_end = args.length();
    pid = args.substr(pid_begin, pid_end);
    
    return 0;
}

void command_kill(string args)
{
    string pid;
    string sig;
    if(parse_kill_args(args, pid, sig) == 0)
    {
        if(kill(atoi(pid.c_str()), atoi(sig.c_str())) < 0)
        {
            
            cout << "Ошибка при отправке сигнала процессу" << endl;
        }
    }
}

void parse_args(string args, string command, char ***parsed_args)
{
    vector<string> temp_args;
    size_t arg_begin, arg_end;
    int i;
    
    size_t pos = 0;
    
    while((arg_begin = args.find_first_not_of(" ", pos)) != std::string::npos)
    {
        pos = arg_begin;
        arg_end = args.find_first_of(" ", pos);
        if(arg_end == std::string::npos)
        {
            arg_end = args.length();
            temp_args.push_back(args.substr(arg_begin, arg_end - arg_begin));
            break;
        }
        temp_args.push_back(args.substr(arg_begin, arg_end - arg_begin));
        pos = arg_end;
    }
    
    *parsed_args = new char*[temp_args.size() + 2];
    
    size_t file_name_begin = command.find_last_of("/");
    if(file_name_begin == std::string::npos)
        file_name_begin = 0;
    
    i = 0;    
    (*parsed_args)[i] = new char[256];
    strcpy((*parsed_args)[i], command.substr(file_name_begin + 1, command.length() - file_name_begin - 1).c_str());
    i++;
    
    for(i ; i < temp_args.size() + 1 ; i++)
    {
        (*parsed_args)[i] = new char[256];
        strcpy((*parsed_args)[i], temp_args[i - 1].c_str());
    }
    (*parsed_args)[i] = NULL;
}

void run_process(string command, string args)
{
    pid_t child_pid;
    char** parsed_args = NULL;
    parse_args(args, command, &parsed_args);
    child_pid = fork();
    if(child_pid == 0){
        if(execvp(command.c_str(), parsed_args) < 0)
        {
            cout << "Ошибка при запуске исполняемого файла" <<endl;
        }
    }
    else{
        wait(NULL);
    }
}

void sigint_handler(int s){
   cout << "Перехвачена комбинация Ctrl-c" << endl;
}

int main(int argc, char** argv)
{
    string input_str, command, args;
    
    signal (SIGINT, sigint_handler);
    
    while(1)
    {
        cout << ">";
        getline(cin, input_str);
        if(parse_command(input_str, command, args) == 0)
        {
            if(command.compare("ls") == 0)
            {
                command_ls();
                continue;
            }
            
            if(command.compare("ps") == 0)
            {
                command_ps();
                continue;
            }
            
            if(command.compare("pwd") == 0)
            {
                command_pwd();
                continue;
            }
            
            if(command.compare("kill") == 0)
            {
                command_kill(args);
                continue;
            }
            
            if(command.compare("exit") == 0)
            {
                break;
            }
            
            if(command.compare("help") == 0)
            {
                print_help();
                continue;
            }
            
            run_process(command, args);
        }
    }
    
    return 0;
}
