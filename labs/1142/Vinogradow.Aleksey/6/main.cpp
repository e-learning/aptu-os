/////////////////////////////////////////////////
//                                             //
//          Vin92.05.05.2014                   //
//                MemoryAllocator              //
//                                             //
/////////////////////////////////////////////////

#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

////////////////////////////////////////////////////////////
//                                                        //
//  +---------------+-------------------+---------+       //
//  |     Данные    |       пусто       | таблица |       //
//                                                        //
//  ^               ^                   ^         ^       //
//  +--- 0          |                   |         |       //
//    p_end_data ---+   p_start_tabl ---+         +--- N  //
//                                                        //
////////////////////////////////////////////////////////////
// подробнее http://yadi.sk/d/1hn_WKGmP8xmz
 
char * memory;
int p_end_data, p_start_tabl, data = 1;
int N;



int my_alloc(int zn){
	if (zn < 1) return -1;

	if ((p_end_data + zn + 2) > p_start_tabl) return -2;
	
	int i = p_end_data;	
	p_end_data += zn;
	
	memory[p_start_tabl - 1]=(p_end_data >> 8) & 0x00ff;
	memory[p_start_tabl - 2]=p_end_data & 0x00ff;
	
	p_start_tabl -= 2;
	
	for (; i < p_end_data; i++) memory[i]=data;
	data++;
	
	return p_start_tabl;
}



int my_free(int zn){
	if (zn < p_start_tabl) return -1;
	if (zn > N) return -2;
	if ((N-zn)%2) return -3;
	
	int adress_end = 0, adress_start = 0;
	
	adress_end=(unsigned char)memory[zn + 1];
	adress_end=adress_end<<8;
	adress_end+=(unsigned char)memory[zn + 0];
	
	
	if (adress_end == 0) return -4;
	
	
	int i = 0;
	 
	
	while (((zn + i) < N) && (adress_start==0)){
		
		adress_start=(unsigned char)memory[zn + i + 3];
		adress_start=adress_start<<8;
		adress_start+=(unsigned char)memory[zn + i + 2];
		
		i+=2;
	}
	
	if ((zn + 2) == N)adress_start=0; 
	
	memory[zn + 1]=0;
	memory[zn + 0]=0;
	
	//cout<<"adress_end:"<<adress_end<<" adress_start:"<<adress_start<<endl;
	
	
	//переписываем таблицу адресов
	i = 2;
	while ((zn - i) >= p_start_tabl){
		int tmp;
		tmp=(unsigned char)memory[zn - i + 1];
		tmp=tmp<<8;
		tmp+=(unsigned char)memory[zn - i + 0];
		//cout<<"tmp:"<<tmp<<endl;
		if (tmp !=0){
			tmp = tmp - adress_end + adress_start;
		
			memory[zn - i + 1]=(tmp >> 8) & 0x00ff;
			memory[zn - i + 0]=tmp & 0x00ff;
		}
		i += 2;
	}
	
	//переписываем данные
	for (i=adress_start; i < p_end_data; i++){
		//cout<<"("<<i<<")"<<(int)(unsigned char)memory[i]<<" ";
		memory[i] = memory[i+(adress_end-adress_start)];
	}
	//cout<<endl;
	
	//новый объём данных
	p_end_data = p_end_data - adress_end + adress_start;
	
	//коррекция объёма таблицы
	adress_start=(unsigned char)memory[p_start_tabl + 1];
	adress_start=adress_start<<8;
	adress_start+=(unsigned char)memory[p_start_tabl];
	
	while ((adress_start==0) && ((p_start_tabl)<N)){
		//cout<<"del"<<endl;
		p_start_tabl+=2;
		
		adress_start=(unsigned char)memory[p_start_tabl + 1];
		adress_start=adress_start<<8;
		adress_start+=(unsigned char)memory[p_start_tabl];
	}
	
	return 1;
}


string s_comm_args(string in_zn, int mode){
	string rez0, rez1;
	char flag=0, flag2=0;
	for (int i=0; i<strlen(in_zn.c_str()); i++){
		if (!flag2){
			 if (in_zn[i] != ' ')
			 	flag2=1;
		}
		if (flag2){
			if (flag){
				rez1+=in_zn[i];
			}else{
				if (in_zn[i] == ' '){ 
					flag = 1;
				}else{
					rez0+=in_zn[i];
				}		
			}
		}
	}
	if (mode) return rez1;
	return rez0;
} 



int main(void){

	cout<<"Hello!"<<endl<<endl<<endl<<"Введите объём всей памяти (N):";
	cin>>N;
	//N=20;
	if (N < 3) return -1;
	if (N >= 50000) return -2;
	memory = new char [N];
	p_end_data=0;	
	p_start_tabl = N;
	
	for (int i=0; i<N; i++) memory[i] = 0;
	
	string command, comm, args;
	int err_code = 0;
	
	while(1){
		getline(cin,command);
		comm = s_comm_args(command, 0);
		args = s_comm_args(command, 1);
		args = s_comm_args(args, 0);
		
		
		if(comm == "exit"){
			break;
		}
		
		
		if(comm == "alloc"){
			err_code = my_alloc(atoi(args.c_str()));
			if (err_code > 0){
				cout<<"+ "<<err_code<<endl;
				err_code = 0;
			}else{
				cout<<"-"<<endl;
			}
		}
		
		
		if(comm == "free"){
			err_code = my_free(atoi(args.c_str()));
			if (err_code > 0){
				cout<<"+"<<endl;
				err_code = 0;
			}else{
				cout<<"-"<<endl;
			}
		}
		
		
		if(comm == "memory"){
			for (int i=0; i < N; i++){
				cout<<(int)(unsigned char) memory[i] <<"   ";
				if (!((i + 1)%16)) cout<<endl;
			}
			cout<<endl;
			cout<<"p_end_data:"<<p_end_data<<endl;
			cout<<"p_start_tabl:"<<p_start_tabl<<endl;
			err_code = 0;
		}
		
		
		if(comm == "info"){
			int i=0, block=0, free = 0;
			while ((p_start_tabl + i) < N){
				if ((memory[p_start_tabl + i] != 0) || (memory[p_start_tabl + i + 1] != 0)) block++;
				i+=2;
			}
			cout<<"Блоков:"<<block<<endl<<"Выделено:"<<p_end_data<<endl;
			free = p_start_tabl - p_end_data - 2;
			if (free < 0) free = 0;
			cout<<"Свободно:"<<free<<endl;
			
			i=p_start_tabl;
			while(i<N){
				int adress_end = 0, adress_start = 0;
		
				adress_end=(unsigned char)memory[i + 1];
				adress_end=adress_end<<8;
				adress_end+=(unsigned char)memory[i + 0];
				
				
				if (adress_end != 0){
					int j = 0;
					
					while (((i + j) < N) && (adress_start==0)){
						
						adress_start=(unsigned char)memory[i + j + 3];
						adress_start=adress_start<<8;
						adress_start+=(unsigned char)memory[i + j + 2];
						
						j+=2;
					}
					
					if ((i + 2) == N)adress_start=0; 
					cout<<"Адрес:"<<i<<" Диапазон памяти:"<<adress_start<<"-"<<adress_end<<"("<<adress_end-adress_start<<")"<<endl;
				}
				i+=2;
			}
			err_code = 0;
		}	

		
		if(comm == "map"){
			for(int i=0; i <N; i++){
				if (i<p_end_data){
					cout<<"u ";
				}else{
					if (i >= p_start_tabl){
						cout<<"m ";
					}else{
						cout<<"f ";
					}
				}
				if (!((i + 1)%16)) cout<<endl;
			}
			cout<<endl;
			err_code = 0;
		}
		
		if((comm == "help") || (comm == "?")){
			cout<<"help - вы не поверите!"<<endl;
			cout<<"alloc Z - выделить Z байт"<<endl;
			cout<<"free Z - удалить блок по адресу Z"<<endl;
			cout<<"info - показать информацию о использованной памяти"<<endl;
			cout<<"map - показать карту пампяти. u - использует пользователь, f - свободно, m - служебные данные"<<endl;
			cout<<"memory - показать память"<<endl;
			cout<<"exit - exit(выход)"<<endl;
		
			err_code = 0;
		}
		
		if (err_code !=0) cout<<"Error:"<<err_code<<endl;
		
		err_code = -10;
		cout<<":";
	}
	
	delete[] memory;
	return 0;
}
