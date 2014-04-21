#include "stdafx.h"

int main(void)
{
	Ps* MMf;
	void *lib=dlopen("./libps.so",RTLD_LAZY);
	if(!lib) return 1;
	Ps* (*MainPs)(void)=(Ps* (*)())dlsym(lib,"GetPs");
	if(MainPs==NULL) cout<<"Trable"<<endl;

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
			Ps* PrintPs=new Ps[1000];
			PrintPs=MainPs();
			//for(int i=0; i<100; i++)
			cout<< PrintPs[1].Name <<" "<<endl;
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
	dlclose(lib);
}
