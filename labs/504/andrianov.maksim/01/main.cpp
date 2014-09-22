#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <cctype>
#include <fstream>
#include <termios.h>
#include "ps.h"

std::string way_val();
int correct_dir(std::string str);
int sign(std::string str);
void pwd(std::string str);
void kill(std::string str,std::string str_signal);
void sys_call(std::string str);
void ctrl_c(int sig);

int main() {
    std::string str_command;
    std::string current;
    current = way_val();
    pid_t pid;

    while(1){
        signal(SIGINT,ctrl_c);

        std::getline (std::cin,str_command);

        std::string str_command_one = "0";
        std::string str_command_two = "0";
        std::string str_command_three = "0";
        std::string str_buffer = "0";

        int index1 = str_command.find_first_of(' ');
        int index2 = str_command.find_last_of(' ');

        if (index1 != -1){
            str_command_one = str_command.substr(0, index1);
            str_command_two = str_command.substr(index1+1, str_command.length() - index1 -1);
            if(index2 != -1 && index1 != index2){
                str_command_two = str_command.substr(index1+1, str_command.length() - index1 - index2);
                str_command_three = str_command.substr(index2+1, str_command.length());
            }
        }
        if(str_command == "ls")
            pwd(current);
        else if(str_command == "exit")
            kill(getppid(), SIGKILL);
        else if(str_command == "ps")
            ps();
        else if(str_command_one== "kill")
            kill(str_command_two, str_command_three);

        else if(str_command == "pwd"){
            std::cout << current << std::endl;
        }
        else if(str_command_one == "cd"){
            if (str_command_two == ".."){
                current = current.substr(0,current.find_last_of('/'));
                if (current.length() < 2)
                    current = "/";
                std::cout << current << std::endl;
            }
            else if (str_command_two[0]=='/'){
                str_buffer = current;
                current = str_command_two;
                if(!correct_dir(str_command_two)){
                    current = str_buffer;
                }
                std::cout << current << std::endl;
            }
            else{
                str_buffer = current;
                current = current + "/" + str_command_two;
                if(!correct_dir(current)){
                    current = str_buffer;
                }
                std::cout << current << std::endl;
            }
        }
        else{
            pid = vfork();
            if(pid == 0){
                sys_call(str_command);   
            }
            else if(pid == -1)
                std::cerr << "Fork failed" << std::endl;

        }
    sleep(0.5);
    }

    return 0;
}


void ctrl_c(int sig){
    if(sig != SIGINT)
        return;
    else{
        std::cout << "You enter ctrl + c" << std::endl;
        return;
        exit (0);
   }
}

void sys_call(std::string str){
    std::string way = "/bin/";
    std::string param1;
    std::string param2;
    std::string param3;
    std::string buffer1;
    int counter = 0;
    int len = str.length();
    for(int i = 0; i < len; i++){
        if(str[i] == ' ')
            counter++;
    }
    switch(counter){
        case 0:
            way = way + str;
            if(execl (way.c_str(), str.c_str(), (char*)0) == -1)
                std::cerr << "Comman not find in /bin/" << std::endl;
            break;
        case 1:
            way = way + str.substr(0,str.find_first_of(' '));
            param1 = str.substr(str.find_first_of(' ')+1,str.length());
            if(execl (way.c_str(), str.substr(0,str.find_first_of(' ')).c_str(),param1.c_str(), (char*)0) == -1)
                        std::cerr << "Comman not find in /bin/" << std::endl;
            break;
        case 2:
            counter = 0;
            for(int i = 0; i < len; i++){
                int cnt = 0;
                if(str[i] == ' ')
                    counter++;
                if(counter == 1){
                    buffer1 = str.substr(0,i);
                    way += way + buffer1;
                    cnt = i;
                }
                if(counter == 2){
                    param1 = str.substr(cnt+1,i);
                    param2 = str.substr(i+1, str.length());
                }
            }
            if(execl (way.c_str(),buffer1.c_str(), param1.c_str(), param2.c_str(), (char*)0) == -1)
                std::cerr << "Comman not find in /bin/" << std::endl;
            break;
        case 3:
            counter = 0;
            for(int i = 0; i < len; i++){
                int cnt = 0;
                if(str[i] == ' ')
                    counter++;
                if(counter == 1){
                    buffer1 = str.substr(0,i);
                    way = way + buffer1;
                    cnt = i;
                }
                if(counter == 2){
                    param1 = str.substr(cnt+1,i);
                    cnt = i;
                }
                if(counter == 3){
                    param2 = str.substr(cnt+1, i);
                    param3 = str.substr(i+1, str.length());
                }

            }
            if(execl (way.c_str(),buffer1.c_str(), param1.c_str(), param2.c_str(), param3.c_str(), (char*)0) == -1)
                std::cerr << "Comman not find in /bin/" << std::endl;
            break;
        default:
            std::cerr << "Uncorrect name command" << std::endl;
    }
}


void pwd(std::string str){
    DIR *dir;
    struct dirent *entry;

    dir = opendir(str.c_str());
    if (!dir) {
        std::cerr << "Crash open dir(pwd)" << std::endl;
        exit(1);
    };
    while ( (entry = readdir(dir)) != NULL) {
        std::cout << entry->d_name << std::endl;
    }
    closedir(dir);
}



int correct_dir(std::string str){
    DIR *dir;
    int status = 1;
    dir = opendir(str.c_str());
    if(!dir)
        status = 0;
    closedir(dir);
    return status;
}



void kill(std::string str,std::string str_signal){
    pid_t pid = atoi(str.c_str());
    kill(pid, sign(str_signal));
}



std::string way_val(){
    std::string str;
    int position = 0;
    str = get_current_dir_name();
    int counter1 = 0;
    for(int i = 0; i < 256; i++){
        if(str[i] == '/'){
            if(counter1 == 2){
                position = i;
                break;
            }
            counter1++;
         }
    }
    str = str.substr(0,position);
    return str;
}

int sign(std::string str){
    if(str == "SIGHUP")
        return 1;
    else if(str == "SIGINT")
        return 2;
    else if(str == "SIGKILL")
        return 9;
    else if(str == "SIGTERM")
        return 15;
    else if(str == "SIGSTOP")
        return 17;
    else{
        std::cerr << "Undifined signal" << std::endl;
        return 0;
    }
}
