/////////////////////////////////
//             LAB3            //
//      Vin92.17.03.2014       //
//                             //
/////////////////////////////////

#include <iostream>
#include "r_a.h"
#include "r_b.h"
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h> 

using namespace std;

int start(int mode){

	cout<<"Запуск процесса.."<<endl;

	//копируем процесс
	pid_t f = fork();

	if (f==-1) return 1;

	if (mode){
		//режим А, потомок в режиме Б
		if (f==0){
			rej_B(0);
			exit(0);
		}	
		rej_A(1);

	}else{
		//режим Б, потомок в режиме А
		if (f==0){
			rej_A(0);
			exit(0);
		}	
		rej_B(1);
	}

	wait(NULL);
	cout<<"Процесс завершён"<<endl;
	return 0;
}


int main(int count_args, char **args){
	
	cout<<endl<<"Hello!"<<endl<<endl;
	//Создаём сегмент в памяти размером в 4 байта
	int id = shmget(ftok("lab3", 0), 0, 0);//проверяем, вдруг кто то уже выделил эту память

	if (id==-1){
		id = shmget(ftok("lab3", 0), sizeof(int), IPC_CREAT | IPC_EXCL | 0x100 | 0x80);

		if (id==-1){cout<<"Не удалось выделить память =\\"<<endl; return 0;}
	}
	cout<<"Адрес выделенной памяти: "<<id<<endl;

	{
		int *adr;
		adr =(int *) shmat(id, NULL, 0);
		*adr=0;
	}

	if (count_args == 1){
		cout<<"Программа запущенна без агрументов."<<endl;
		start(1);

	}else{	
		for (int i=0; i<count_args; i++){
			cout<<"аргумент "<<i<<": "<<args[i]<<endl;
			if ((args[i][0]=='-') && (args[i][1]=='b') && (args[i][2]==0)){
				if(start(0)) cout<<"Ошибка"<<endl;
				break;
			}
			if ((args[i][0]=='-') && (args[i][1]=='a') && (args[i][2]==0)){
				if(start(1)) cout<<"Ошибка"<<endl;
				break;
			}
		}
	}

	//удаляем сегмент из памяти
	shmctl(id, IPC_RMID, NULL);

	return 0;//Выходим и радуемся
}  
