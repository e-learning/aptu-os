#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <dirent.h>
using namespace std;

int main(void)
{
        cout<<"Привет, "<<getenv("USERNAME")<<"!"<<endl;        

        string command,argument;
	char qstring[50];
	char sep[10]=" ";
	while (1)
	{
	cout<<">";
	cin>>qstring;
	command=strtok(qstring,sep);
               
                if(command=="help")
                {
                        cout    <<"help - помощь"<<endl
                                <<"ls - вывод списка файлов в текущем каталоге"<<endl
                                <<"pwd - вывод полного имени текущего каталога"<<endl
                                <<"ps - вывод списка процессов"<<endl
                                <<"kill - отправка сигнала с заданным номером конкретному процессу"<<endl
                                <<"exit - выход"<<endl;
                }
                if(command=="ls")
                {
                        DIR *Dir = opendir(".");
                        if(!Dir) return 1;

                        struct dirent *entry;
                        while((entry=readdir(Dir))!=NULL)
                                cout<<entry->d_name<<endl;
                        closedir(Dir);
                }
                if(command=="pwd") cout<<getenv("PWD")<<endl;
		if(command=="ps")
		{
			DIR *Dir = opendir("/proc");
			if(!Dir) return 1;

			struct dirent *entry;
			while((entry=readdir(Dir))!=NULL)
			{
				string InfName = "/proc/"+string(entry->d_name)+"/comm";
				FILE *Inf=fopen(InfName.c_str(),"r");
				if(Inf==NULL) return 1;
				char name[40];
				fgets(name,40,Inf);
				cout<<entry->d_name<<" "<<name<<endl;
				fclose(Inf);
			}
			closedir(Dir);
		}
		if(command=="kill")
		{
			argument=strtok(NULL,sep);
			//kill(atoi(argument.c_str()),0);
		}               
                if(command=="exit") break;
        }
}

