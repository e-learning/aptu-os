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

	string command;
	while (1)
	{
		cout<<">";
		cin>>command;
		
		if(command=="help")
		{
			cout	<<"help - помощь"<<endl
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
				FILE *Inf=fopen(("/proc/"+entry->d_name+"/comm"),"r");
				if(Inf==NULL) return 1;
				string name;
				getline(Inf,name);
				cout<<entry->d_name<<" "<<name<<endl;
				fclose(Inf);
			}
		}
				
		if(command=="exit") break;
	}
}
