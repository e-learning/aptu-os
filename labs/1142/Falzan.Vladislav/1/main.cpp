using namespace std;
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>

void Menu();
int Action();
void obr(int);
int Kill(string,int);
int Ps();


void Menu()
{
	printf("-------------------");
	printf("\nstart name_file\n");
	printf("ls\n");
	printf("pwd\n");
	printf("ps\n");
	printf("kill -name_file -signal\n");
	printf("exit\n");
	printf("help\n");
	printf("-------------------\n\n"); 
}

int Action()
{
	string in;
	getline(cin,in);

    while(in[0] == ' ')
    	{
		in=in.substr(1,in.length()-1);
	}
	while(in[in.length()-1] == ' ')
	{
		in=in.substr(0,in.length()-1);
	}
	
	int space = in.find(" ");
	string com = in.substr(0,space);

	if (space == -1)
	{
		if (in == "ls")
		{
			DIR *cur_dir = opendir(".");
	        	if(!cur_dir)
	        	{
				printf("Error: Can't open current directory. (ls)");
				return 1;
			}
	        
	        struct dirent *entry;

	        while ((entry = readdir(cur_dir)) != NULL)
				printf("%s\n", entry->d_name); 
	        closedir(cur_dir);
	        return 0;
		}
		
		if (in == "pwd")
		{
			printf("Current directory: %s \n",get_current_dir_name());
			return 0;
		}
		
		if (in == "ps")
		{
			return Ps();
		}

		if (in == "exit")
		{
			exit(0);
			return 0;
		}
		if (in == "help")
		{
			Menu();
			return 0;
		}
	}
	else
	{
		string arg = in.substr(space+1,in.length()-space);
		
		if (com == "start")
		{
			if(system(arg.c_str()))
			{
				printf("Error: Can't open file. (Start)\n");
				return 0;
			}
			return 0;
		}

		if (com == "kill")
		{
			return Kill(arg,space);
		}
		else
		{
			printf("Error: Unknown command. (Action)\n");
			return 1;
		}
	}
}

void  obr(int sig)
{
	printf("You entered Ctrl + C\n");
}

int Kill(string arg,int space)
{
	string arg1,arg2;
	while(arg[0] == ' ')
	    {
		arg = arg.substr(1,arg.length()-1);
       	    }
	while(arg[arg.length()-1] == ' ')
	{
		arg = arg.substr(0,arg.length()-1);
	}
	space = arg.find(' ');

	arg1 = arg.substr(1,space-1);
	space = arg.rfind('-');
	arg2 = arg.substr(space+1,arg.length()-space-1);
	if (kill(atoi(arg1.c_str()),atoi(arg2.c_str())))
		return 0;
	else
	{
		printf("Error: Can't kill this process. (kill)\n");
		return(1);
	}
}

int Ps()
{
	DIR *proc_dir = opendir("/proc");
			
	struct dirent *entry;
	string file_name;
 
	printf("%5s %5s\n","PID","NAME");
	if (proc_dir)
        while((entry = readdir(proc_dir)) != NULL)
        {
                if ((entry->d_name[0] <= '9')&&(entry->d_name[0] >= '0'))
                {
                        file_name = "/proc/" + string(entry->d_name) + "/comm";
                        		                        
                        ifstream file(file_name.c_str());

                        string proc_name;
			getline(file, proc_name);
                        		                        
                        printf("%5s %s\n", entry->d_name, proc_name.c_str());
                }
        }
	else
	{
		printf("Error: Can't open ""/proc"" directory. (ps)\n");
		return 1;
	}
	return 0;
}

int main(void)
{
	signal(SIGINT, obr);
	printf("\nLaba 1.\n");
	printf("Welcome!\n");
	Menu();
	while (1)
	{
		if (Action())
			{
				printf("Error. (Action)\n");
				exit(1);
			}
	}
	return 0;
}
