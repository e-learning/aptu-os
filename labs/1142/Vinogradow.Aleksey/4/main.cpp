/////////////////////////////////
//             LAB1            //
//      Vin92.21.02.2014       //
//                             //
/////////////////////////////////

#include <iostream>
#include "comminterp.h"

using namespace std;

int main(void){
	
	cout<<"Hello, "<<getenv("USERNAME")<<"!"<<endl;

	cout<<"Программа выплнена на ""3"", т.е. реализованы только пункты ""1"" и ""3"""<<endl;

	cout<<endl<<endl<<"Введите объём всей памяти (N):";
	int N, block=0, size = 0;
	cin>>N;

	cout<<endl<<"Ok, босс, теперь у нас целых "<<N<<" байт памяти!"<<endl;


	string command, comm, args;

	int err_code = 0;

	//вечный цикл здесь=/
	while(1){
		getline(cin,command);

		comm = s_comm_args(command, 0);
		args = s_comm_args(command, 1);
		args = s_comm_args(args, 0);

		if (comm=="exit") break;	
		
		if ((comm=="help" || command=="?")){ cout<<"help - Вы не поверите.."<<endl<<"exit - выход"<<endl<<"info - информация о памяти"<<endl<<"alloc N - выделить N байт"<<endl; err_code = 0;}

		if(comm=="info"){
			cout<<"В нашем распоряжении осталось "<<N<<" байт."<<endl<<"Выделено блоков: "<<block<<"."<<endl<<"Всего выделенной памяти: "<<size<<"."<<endl;
			err_code = 0;
		} 

		if(comm=="alloc"){          //    На лекции было сказано,
			N-=atoi(args.c_str());         //   что память можно даже не выделять. 
			if (N<0){
				cout<< "-"<<endl;
				N+=atoi(args.c_str());
				err_code = 1;
			}else{		 	
				block ++;      	
				err_code = 0;
				size +=atoi(args.c_str());
				cout<<" + "<<size<<endl;
			}
		}

		if (err_code>0) cout<<"error: "<<err_code<<endl;
		if (err_code==-1) cout<<"Kажется, что данная команда не поддерживается =/"<<endl;
		err_code = -1;
		cout<<":";
	}
	
	cout<<"Удачи!"<<endl;
	return 0;
}
