#include "r_a.h"

using namespace std;

int rej_A(int z){
	cout<<"Процесс";    
	if (!z) cout<<"-потомок";
	cout<<" работает в режиме 'A'. PID:"<<getpid()<<endl;

	srand(time(NULL));


	int id = shmget(ftok("lab3", 0), 0, 0);
	if (id==-1){cout<<"А: Не удалось получить адрес =\\"<<endl; return 2;}
	cout<<"А: Адрес выделенной памяти: "<<id<<endl;

	int * adr;

	adr =(int *) shmat(id, NULL, 0);
	if (adr==(int *)-1){cout<<"А: Ошибка при получении адреса."<<endl; return 1;}

	while (*adr!=-1){
	
		if ((*adr!=0)&&(*adr!=-1)) {
			cout<<"A: В памяти найдено число:"<<*adr<<". Можно поспать..."<<endl;
			sleep(rand()%10);
			cout<<"А: Проснулись и убрали число "<<*adr<<"."<<endl;
			*adr=0;
		}
	}

	cout<<"А: 'B' сообщил, что можно сваливать."<<endl;
	return 0;
}
