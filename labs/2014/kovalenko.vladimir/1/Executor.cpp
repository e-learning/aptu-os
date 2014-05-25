#include "Executor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/param.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <algorithm>
#include <iterator>

bool isNumber(const std::string& str)
{
    std::string::const_iterator it = str.begin();
    if(*it == '-'){
        it++;
    }
    while (it != str.end() && std::isdigit(*it)) ++it;
    return (!str.empty() && (it == str.end()));
}

bool Executor::execute(Command const & c){
	 
	if (c.cmd == "exit")
        return false;
	if (c.cmd.empty())
        return true;
    if (c.cmd == "pwd")
        executePwdCommand(c);
    else if (c.cmd == "ls")
        executeLsCommand(c);
    else if (c.cmd == "ps")
        executePsCommand(c);
    else if (c.cmd == "kill")
        executeKillCommand(c);
    else
        executeOtherCommand(c);

    return true;
}

void Executor::executeKillCommand(Command const & cmd){
	if (cmd.args.size() != 2 || !isNumber(cmd.args[0]) || !isNumber(cmd.args[1])) {
    	std::cerr << "'kill' command requires two numeric arguments" << std::endl;
    	return;
    }
    kill(atoi(cmd.args[1].c_str()), atoi(cmd.args[0].c_str()));
}

void Executor::executePsCommand(Command const & cmd){
    if (!cmd.hasNoArguments)
    {
        std::cerr << "'ps' command takes no arguments" << std::endl;
        return;
    }
    showProcessList();
}

void Executor::showProcessList(){
    DIR *procdir;
    std::string pathtoprocdir = "/proc/";
    if ((procdir = opendir(pathtoprocdir.c_str())) == 0)
        std::cout << "failed to read /proc/, are you using OS X?" << std::endl;
    else {
        dirent *pid;
        std::string commandLine;
        while ((pid = readdir(procdir)) != 0) {
            if (pid->d_type == DT_DIR && isNumber(pid->d_name)) {
                std::string cmdFilePath = pathtoprocdir + std::string(pid->d_name) + "/comm";
                std::ifstream cmdFile(cmdFilePath.c_str());
                if (cmdFile.is_open()) {
                    getline(cmdFile, commandLine);
                    std::cout << pid->d_name << ": " << commandLine << std::endl;
                }
                cmdFile.close();
            }
        }
        closedir(procdir);
    }
}

void Executor::executeLsCommand(Command const & cmd){
    if (!cmd.hasNoArguments)
    {
        std::cerr << "'ls' command takes no arguments" << std::endl;
        return;
    }
    listFiles();
}

void Executor::listFiles(){
    DIR *dir = opendir(".");
    if (!dir) {
        std::cerr << "Error: failed to execute opendir()" << std::endl;
        return;
    }
    dirent *de;
    std::vector<std::string> fileNames;
    while ((de = readdir(dir)) != 0) {
        fileNames.push_back(de->d_name);
    }
    std::sort(fileNames.begin(), fileNames.end());
    std::copy(fileNames.begin(), fileNames.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
    closedir(dir);
}


void Executor::executePwdCommand(Command const & cmd){
    if (!cmd.hasNoArguments)
    {
        std::cerr << "'pwd' command takes no arguments" << std::endl;
        return;
    }
    showDirPath();
}

void Executor::showDirPath(){
    char *pwd = getcwd(NULL, 0);
    std::cout << pwd << std::endl;
    free(pwd);
}

void Executor::executeOtherCommand(Command const & cmd){
    std::string raw = cmd.cmd;
    for(std::vector<std::string>::const_iterator it = cmd.args.begin(); it!=cmd.args.end(); it++) raw += " " + *it;
    system(raw.c_str());
}


