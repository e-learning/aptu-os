#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

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

	void *lib = dlopen("./libps.so", RTLD_LAZY);
	void (*Ps)(string *);
	
	if (!lib) {
		cout<<"Error load lib!"<<endl;
		return 1;
	}

	Ps = (void (*)(string *))dlsym(lib, "Ps");
	if (Ps==NULL) {
		cout<<"Function not found"<<endl;
		return 2;
	}

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
