#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>
using namespace std;

void Commands();
int Functions();
void obr(int);
int Kill(string,string);
int Ps();


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
			return Ps();
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

int Ps()
{
	DIR *UseDirection = opendir("/proc/");		
	struct dirent *start;
	string Name_File;
 
	printf("%5s %5s\n","PID","NAME");
	if (UseDirection)
        while((start = readdir(UseDirection)) != NULL)
        {
                if ((start->d_name[0] <= '9')&&(start->d_name[0] >= '0'))
                {
                        Name_File = "/proc/" + string(start->d_name) + "/comm";
                        		                        
                        ifstream file(Name_File.c_str());

			string NameProcess;
			getline(file, NameProcess);
                        		                        
                        printf("%5s %s\n", start->d_name, NameProcess.c_str());
                }
        }
		else
		{
			cout<<"Error: Can't open ""/proc"" directory. (ps)\n";
			return 1;
		}
	return 0;
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

