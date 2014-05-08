#include "r_b.h"

using namespace std;

int rej_B(int z){
	cout<<"Процесс";    
	if (!z) cout<<"-потомок";
	cout<<" работает в режиме 'B'. PID:"<<getpid()<<endl;

	int id = shmget(ftok("lab3", 0), 0, 0);

	if (id==-1){cout<<"B: Не удалось получить адрес =\\"<<endl; return 2;}
	cout<<"B: Адрес выделенной памяти: "<<id<<endl;


	int * adr;

	adr =(int *) shmat(id, NULL, 0);
	if (adr==(int *)-1){cout<<"B: Ошибка при получении адреса."<<endl; return 1;}

	srand(time(NULL));

	int rand_zn;

	int count=0;

	while (1){
		if (*adr==0){
			count ++;
			if (count>8){
				break;			
			}
			rand_zn = rand()%1000+1;
			cout<<"B: Записали число("<<count<<"): "<<rand_zn<<endl<<"B: Спим.."<<endl;
			*adr=rand_zn;
			sleep(1);	
		}
		cout<<"B: Опять спим..."<<endl;
		sleep(rand()%5);
	}
	cout<<"B: 'А' отчистил память. Завершаем работу :)"<<endl;
	*adr=-1;

	return 0;
}
