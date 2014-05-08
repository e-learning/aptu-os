#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "ps.h"

using namespace std;

void Signal (int sig){
	if (sig == SIGINT) cout<<"Ctrl-c"<<endl;
}

void Ls(void){
	DIR *directory = opendir(".");
	if(!directory) return;
	struct dirent *f_directory;
	while ((f_directory=readdir(directory))!=NULL)cout<< f_directory->d_name <<endl; 
	closedir(directory);
}

int main(void){

	cout<<"Hello!"<<endl;

	string command;

	signal(SIGINT, Signal);

	while(1){
		cout<<">>";
		getline(cin,command);

		if (command == "exit") return 0;

		if (command == "ls"){
			Ls();
			command = " ";	
		}

		if (command == "kill"){
			int p, s;
			cout<<"PID:";
			cin>>p;
			cout<<"S:";
			cin>>s;
			kill(p, s);
			command = " ";
		}

		if (command == "ps"){
			string tmp="";
			Ps(&tmp);
			cout<<tmp;
			command = " ";
		}

		if (command == "pwd"){
			cout<<getenv("PWD")<<endl;
			command = " ";
		}

		if (command != " ") system(command.c_str());

	}


	return 0;
}
