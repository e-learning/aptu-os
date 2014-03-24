using namespace std;
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>
#include <dlfcn.h>

void Menu();
int Action();
void obr(int);
int Kill(string,string);
//int Ps();
void *lib;
int (*Ps)(string *);


void Menu()
{
	printf("-------------------");
	printf("\n1 - start name_file\n");
	printf("2 - ls\n");
	printf("3 - pwd\n");
	printf("4 - ps\n");
	printf("5 - kill name_file signal\n");
	printf("6 - exit\n");
	printf("7 - help\n");
	printf("-------------------\n\n"); 
}

int Action()
{

	int in;
	cin >> in;
	
		if (in == 1)
		{
			string arg;
			cin >> arg;
			if(system(arg.c_str()))
			{
				printf("Error: Can't open file. (Start)\n");
				return 0;
			}
			return 0;
		}

		if (in == 2)
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
		
		if (in == 3)
		{
			printf("Current directory: %s \n",get_current_dir_name());
			return 0;
		}
		
		if (in == 4)
		{
			string resultPs;
			Ps(&resultPs);
			cout << resultPs;
		}

		if (in == 5)
		{
			string arg1,arg2;
			cin >> arg1;
			cin >> arg2;
			return Kill(arg1,arg2);
		}

		if (in == 6)
		{
			exit(0);
			return 0;
		}
		if (in == 7)
		{
			Menu();
			return 0;
		}

		else
		{
			printf("Error: Unknown command. (Action)\n");
			return 1;
		}
}

void  obr(int sig)
{
	printf("You entered Ctrl + C\n");
}

int Kill(string arg1,string arg2)
{
	if (kill(atoi(arg1.c_str()),atoi(arg2.c_str())))
		return 0;
	else
	{
		printf("Error: Can't kill this process. (kill)\n");
		return(1);
	}
}

int main(void)
{
	signal(SIGINT, obr);
	printf("\nLaba 1.\n");
	printf("Welcome!\n");
	
	lib = dlopen("./libps.so", RTLD_LAZY);

	if (!lib) {
		cout<<"Library's Error."<<endl;
	}else{
		Ps = (int (*)(string *))dlsym(lib, "Ps");
		if (Ps == NULL) {
			cout<<"Функция не найдена"<<endl;
				}
		  }

	Menu();
	while (1)
	{
		if (Action())
			{
				exit(1);
			}
	}
	return 0;
}

