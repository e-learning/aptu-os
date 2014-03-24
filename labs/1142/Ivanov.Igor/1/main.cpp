#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
using namespace std;

int main(void)
{
        //cout<<"Привет, "<<getenv("USERNAME")<<"!"<<endl;       

        string command,argument;
	string qstring;
	char qchar[50];
	while (1)
	{
	cout<< ">";
	getline(cin,qstring);
	memcpy(qchar,qstring.c_str(),50);
	command=strtok(qchar," ");

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
				if((entry->d_name[0]<='9')&&(entry->d_name[0]>='0'))
				{
					string InfName = "/proc/"+string(entry->d_name)+"/comm";
					FILE *Inf=fopen(InfName.c_str(),"r");
					if(Inf==NULL) return 2;
					char name[40];
					fgets(name,40,Inf);
					cout<< entry->d_name <<" "<< name <<endl;
					fclose(Inf);
				}
			closedir(Dir);
		}
		if(command=="kill")
		{
			argument=strtok(NULL," ");
			kill((atoi(argument.c_str())),SIGKILL);
		}
                if(command=="exit") break;
		if((command!="exit")&&(command!="help")&&(command!="ls")&&
		(command!="ps")&&(command!="kill")&&(command!="pwd"))
		system(command.c_str());
        }
}
