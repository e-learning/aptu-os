#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>
#include <dlfcn.h>
#include <unistd.h>
#include "ps.h"
using namespace std;

void Commands();
int Functions();
void obr(int);
int Kill(string,string);

string finalyPs;

void Commands()
{
	cout<<"\nCommands:"<<endl;
	cout<<"file"<<endl;
	cout<<"ls"<<endl;
	cout<<"pwd"<<endl;
	cout<<"ps"<<endl;
	cout<<"kill name_file signal"<<endl;
	cout<<"exit"<<endl;
	cout<<"help"<<endl; 
}

int Functions()
{
	string op;
	cin>>op;
	
		if (op == "file")
		{
			string arg;
			cout<<"Input name file: ";
			cin >> arg;
			if(system(arg.c_str()))
			{
				cout<<"Can't open file!\n";
				return 0;
			}
			return 0;
		}

		if (op == "ls")
		{
			DIR *ExecuteDirectory = opendir(".");
	        	if(!ExecuteDirectory)
	        	{
				cout<<"Error: Can't open executable directory. (ls)\n";
				return 1;
			}
	        
	        struct dirent *start;

	        while ((start = readdir(ExecuteDirectory)) != NULL)
				cout<<start->d_name<<endl;
	        closedir(ExecuteDirectory);
	        return 0;
		}
		
		if (op == "pwd")
		{
			cout<<"Executable directory: "<<get_current_dir_name()<<endl;
			return 0;
		}
		
		if (op == "ps")
		{
			Ps(&finalyPs);
			if (finalyPs == "n")
				return 1;
			else
			{
				cout << "Result function Ps =" << finalyPs << "\n";
				return 0;
			}
		}

		if (op == "kill")
		{
			string Name_File,Signal;
			cout<<"Input name file: ";
			cin >> Name_File;
			cout<<"\nInput signal: ";
			cin >> Signal;
			return Kill(Name_File,Signal);
		}

		if (op == "exit")
		{
			exit(0);
			return 0;
		}
		if (op == "help")
		{
			Commands();
			return 0;
		}

		else
		{
			cout<<"Unknown command.\n";
			return 1;
		}
}

void  obr(int sig)
{
	cout<<"You entered Ctrl + C\n";
}

int Kill(string Name_File,string Signal)
{
	if (kill(atoi(Name_File.c_str()),atoi(Signal.c_str())))
	{
		cout<<"Process exit!\n";
		return 0;
	}
	else
	{
		cout<<"Can't kill this process!!!\n";
		return(1);
	}
}


int main(void)
{
	signal(SIGINT, obr);

	Commands();
	cout<<"\nInput operation:"<<endl;
	while (1)
	{
		if (Functions())
			{
				exit(1);
			}
	}
	return 0;
}

