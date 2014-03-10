/////////////////////////////////
//             LAB1            //
//      Vin92.21.02.2014       //
//                             //
/////////////////////////////////

#include <iostream>
#include "comminterp.h"
#include <stdlib.h>
#include <signal.h>
#include <dlfcn.h>
#include <stdio.h>

using namespace std;

void obrab_sig(int zn){
	printf("\rКажется, что вы нажали сочетание Ctrl-c\n\r: ");
}

int main(void){
	
	cout<<"Hello, "<<getenv("USERNAME")<<"!"<<endl;
 
	//пытаемся подгрузить библиотеку
	void *lib;
	int (*fPS)(string *);
	int load_lib_flag=0;
	
	lib = dlopen("./libps.so", RTLD_LAZY);

	if (!lib) {
		cout<<"Не удалось подгрузить библиотеку :("<<endl;
		load_lib_flag = 1;
	}else{
		//вычисление адреса функции
		fPS = (int (*)(string *))dlsym(lib, "fPS");
		if (fPS==NULL) {
			cout<<"Функция не найдена"<<endl;
			load_lib_flag = 2;
		}
	}


	string command, comm, args, rez_fPS;
	int err_code;
	
	signal(SIGINT, obrab_sig);

	//вечный цикл здесь=/
	while(1){
		cout<<":";
		getline(cin,command);
		cout<<"-----------"<<endl;
			
		err_code = -1;

		comm = s_comm_args(command, 0);
		args = s_comm_args(command, 1);

		if (comm=="exit") break;	
		
		if ((comm=="help" || command=="?")){ cout<<"help - помощь"<<endl<<"exit - выход"<<endl<<"ls - файлы текущей директории"<<endl<<"pwd - текущее место"<<endl<<"ps - список процессов"<<endl<<"kill номер_процесса сигнал - послать сигнал процессу"<<endl<<"run исполняемый_с_аргументами - запустить файл"<<endl; err_code = 0;}

		if (comm=="ls") err_code = fLS();

		if (comm=="pwd") { err_code = 0; cout<<getenv("PWD")<<endl; } 

		if ((comm=="ps")&&(!load_lib_flag)) { err_code = fPS(&rez_fPS); cout<<rez_fPS; }

		if (comm=="kill") err_code = fKILL(args);

		if (comm=="run") {err_code = 0; system(args.c_str()); }

		cout<<"-----------"<<endl;

		if (err_code>0) cout<<"error: "<<err_code<<endl;
		if (err_code==-1) cout<<"Kажется, что данная команда не поддерживается =/"<<endl<<"Либо используете команду run"<<endl;
	}
	
	if (!load_lib_flag) dlclose(lib);

	cout<<"До встречи!"<<endl;
	return 0;
}
