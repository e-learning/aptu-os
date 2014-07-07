#include "shell.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <csignal>
#include <fnmatch.h>

using std::cout;
using std::endl;
using std::cin;
using std::vector;
using std::sort;
using std::pair;
using std::ifstream;

void Shell::run(){
    string command;
    while(true){
        cout << curDir.getCurDir() << " > ";
        getline(cin, command);
        command = utils::trim(command);
        if(!executeCommand(command)) break;
    }
}

bool Shell::executeCommand(string const command){
    if("exit" == command) return false;
    else if(0 == command.find("cd")){
            string arg = command.substr(2, command.size() - 1);
            if(".." == utils::trim(arg)) curDir.dirLevelUp();
            else curDir.changeCurDir(arg);
    }
    else if(0 == command.find("ls")) ls();
    else if(0 == command.find("pwd")) pwd();
    else if(0 == command.find("ps")) ps();
    else if(0 == command.find("kill")) killF(command);
    else{
        if(0 != system(command.c_str())) cout << "Shell error: no such command" << endl;
    }
    return true;
}

void Shell::ls(){
    vector<string> files;
    DIR *d;
    dirent *dir;
    d = opendir(".");
    if(d){
        while((dir = readdir(d))) files.push_back(dir->d_name);
        closedir(d);
    }
    sort(files.begin(), files.end());
    for(vector<string>::iterator it = files.begin(); it != files.end(); ++it)
        cout << *it << endl;
}

void Shell::pwd(){
    cout << curDir.getCurDir() << endl;
}

void Shell::ps(){
    DIR *d;
    dirent *dir;
    d = opendir("/proc");
    ifstream in;
    if(d){
        while((dir = readdir(d))){
            if(string(dir->d_name).find_first_not_of("0123456789") == string::npos){
                in.open(string("/proc/" + string(dir->d_name) + "/comm").c_str());
                string processName("");
                in >> processName;
                in.close();
                cout << string(dir->d_name) << " " << processName << endl;
            }
        }
        closedir(d);
    }
}

void Shell::killF(string command){
    string args = command.substr(4, command.size() - 1);
    args = utils::trim(args);
    if(string::npos == args.find(" ")){
        cout << "Error! Using: kill signal processID" << endl;
        return;
    }
    string signal = args.substr(0, args.find(" "));
    string process = args.substr(args.find(" ") + 1, args.size() - 1);
    if(-1 == kill(atoi(process.c_str()), atoi(signal.c_str()))) cout << "Error! Using: kill signal processID" << endl;
}
