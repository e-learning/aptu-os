/////////////////////////////////////////////////
//                                             //
//          Vin92.05.05.2014                   //
//                MemoryAllocator2             //
//                                             //
/////////////////////////////////////////////////

#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

 
char * memory;
int p_end_data, p_start_tabl, data = 1;
int N;


int searh(int zn){
	int size, adress_start, i;
	
	for (i=p_start_tabl; i<N; i+=4){
		adress_start=(unsigned char)memory[i + 3];
		adress_start=adress_start<<8;
		adress_start+=(unsigned char)memory[i + 2];
		
		
		size=(unsigned char)memory[i + 1];
		size=size<<8;
		size+=(unsigned char)memory[i + 0];
		if (adress_start == zn){
			return size;
		}
	}
	return 0;
}


int searh_free(int zn){
	int adress_start = 0, adr, ch = 0, adress_end_max = 0, size;
	
	for (int i=p_start_tabl; i<N; i+=4){
		adress_start=(unsigned char)memory[i + 3];
		adress_start=adress_start<<8;
		adress_start+=(unsigned char)memory[i + 2];
		
		size=(unsigned char)memory[i + 1];
		size=size<<8;
		size+=(unsigned char)memory[i + 0];
		
		
		int adress_end = size + adress_start;
		if (adress_end_max < adress_end) adress_end_max = adress_end;
	}
	if ((adress_end_max + 4)>p_start_tabl) return -3; 
	
	adress_start = 0;
	
	int i=0;	
	while (i <= p_start_tabl - 4){
		adr = searh(i);
				
		if (adr){ 
			i+=adr;
			adress_start = i;
			ch = 0;
		}else{
			ch++;
			i++; 
			if ((ch >= zn)&&((adress_start + zn + 4) <= p_start_tabl)) return adress_start;
		}
	}
	
	return -1;
}


int my_alloc(int zn){
	if (zn<1) return -1;
	int start = searh_free(zn);
	if (start < 0) return start*10;
	
	memory[p_start_tabl - 1]=(start>> 8) & 0x00ff;
	memory[p_start_tabl - 2]=start & 0x00ff;
	memory[p_start_tabl - 3]=(zn >> 8) & 0x00ff;
	memory[p_start_tabl - 4]=zn & 0x00ff;
	
	p_start_tabl -= 4;
	
	for (int i=0; i < zn; i++) memory[i+start]=data;
	data++;

	return start;
}


int my_free(int zn){
	int size, adress_start, i;
	
	int rez=-1;
	
	for (i=p_start_tabl; i<N; i+=4){
		adress_start=(unsigned char)memory[i + 3];
		adress_start=adress_start<<8;
		adress_start+=(unsigned char)memory[i + 2];
		
		if (adress_start == zn){
			rez = 0;
			
			size=(unsigned char)memory[i + 1];
			size=size<<8;
			size+=(unsigned char)memory[i + 0];
			
			for (int j=0; j < size; j++){
				memory[j+adress_start]=0;
			}
			
			break;
		}
	}
	
	if (!rez){
		for(;i>p_start_tabl;i-=4){
			memory[i + 3] = memory[i - 1];
			memory[i + 2] = memory[i - 2];
			memory[i + 1] = memory[i - 3];
			memory[i + 0] = memory[i - 4];
		}
		p_start_tabl +=4;
		
		memory[i + 3] = 0;
		memory[i + 2] = 0;
		memory[i + 1] = 0;
		memory[i + 0] = 0;
	}
	return rez;
}


int info(){

	int all_size = 0, ch = 0, size;

	for (int i=p_start_tabl; i<N; i+=4){
		
		size=(unsigned char)memory[i + 1];
		size=size<<8;
		size+=(unsigned char)memory[i + 0];
		
		ch ++;
		
		all_size += size;
	}

	cout<<"Блоков: "<<ch<<endl<<"Выделено: "<<all_size<<endl;
	
	int i = p_start_tabl;
	
	while((searh_free(i) < 0) && i > 0) i--;

	cout<<"Cвободно: "<<i<<endl;

	return 0;
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
	//N=32;
	if (N < 5) return -1;
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
			if (err_code >= 0){
				cout<<"+ "<<err_code<<endl;
			}else{
				cout<<"-"<<endl;
			}	
			err_code = 0;
		}
		
		
		if(comm == "free"){
		err_code = my_free(atoi(args.c_str()));
			if (err_code >= 0){
				cout<<"+"<<endl;
			}else{
				cout<<"-"<<endl;
			}	
			err_code = 0;
		}
		
		if(comm == "info"){
			err_code = info();
		}
		
		if(comm == "map"){
			err_code = 0;
			
			string map = "";
			
			for (int i=0; i < N; i++){
				map+='u';
			}
			
			for (int i=p_start_tabl; i < N; i++){
				map[i]='m';
			}
			
			int i=0;	
			while (i < p_start_tabl){
				int adr = searh(i);
						
				if (adr){ 
					i+=adr;
				}else{
					map[i]='f';
					i++; 
				}
			}
			
			cout<<map<<endl;
			
		}
		
		
		if(comm == "memory"){
			for (int i=0; i< N; i++){
				cout<<(int)(unsigned char)memory[i]<<" ";
			}
			cout<<endl;
			err_code = 0;
		}
	
	
		if (err_code<0){
			cout<<"Error "<<err_code<<endl;
		}
		
		
		err_code = -10;
		cout<<":";
	
	}
	
	delete[] memory;	
	return 0;
}	
