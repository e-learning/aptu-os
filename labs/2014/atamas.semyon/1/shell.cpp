//============================================================================
// Name        : shell.cpp
// Author      : atamas
// Version     :
// Copyright   : Your copyright notice
// Description : Shell command interpriter
//============================================================================

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
using std::string;
using std::cin;
using std::cout;

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void parseCommand(){
	char in[256];
	cin.getline(in, 256);
	std::istringstream instream(in);
	std::string command;
	instream >> command;

	if(command == "ls"){
		char path[256];
		getcwd(path, 256);
		DIR* currDir = opendir(path);
		dirent * file;
		while( (file = readdir(currDir)) !=0 ){
			cout << file->d_name << std::endl;
		}
		closedir(currDir);
	}else if(command == "pwd"){
		char path[256];
		getcwd(path, 256);
		cout << path << std::endl;
 	}else if(command == "kill"){
		int procNo, sig;
		instream >> sig;
		instream >> procNo;
		kill(procNo, -sig);
	}else if(command == "exit"){
		exit(0);
	}else if(command == "ps"){
		char path[256] = "/proc";
		DIR* currDir = opendir(path);
		dirent * file;
		while( (file = readdir(currDir)) !=0 ){
			if(file->d_type == 4 && isNumber(file->d_name)){
				std::string procStatusFilePath = "/proc/"+std::string(file->d_name)+"/status";
				std::ifstream procStatusFile;
				procStatusFile.open(procStatusFilePath.c_str(), std::ifstream::in);
				std::string name;
				procStatusFile >> name;
				procStatusFile >> name;
				cout << file->d_name << " " << name << std::endl;
			}
		}
		closedir(currDir);
	}else{
		std::vector<char *> argv;
		argv.push_back("Name");
	    char buff[256];
		while(instream >> buff){
			argv.push_back(buff);
		}
		argv.push_back(NULL);
		pid_t pid = fork();
		int status;
		switch(pid){

		case 0:{
			execvp(command.c_str(), &argv[0]);
			std::cout << "can't find process " << command << std::endl;
			exit(0);
		}
		break;

		default:
			while(-1 == waitpid(pid, &status, 0));
		break;
		}
	}
}


int main() {
	while(true){
		parseCommand();
	}
	return 0;
}
