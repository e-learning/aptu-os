#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;


int Memory_Size(int ssize){
	cout << "Blocks Space=";
	cin >> ssize;
	return ssize;
}

int main(void)
{
	int res_adr = 0;
	int sys = 16;//Ðåçåðâèðóåì ïîñëåäíèå 16 áàéò ïîä ñëóæåáíûå äàííûå
	int N = 0;//Ðàçìåð âûäåëÿåìîé ïàìÿòè
	int global_size = 0, block = 0, size = 0;
	N = Memory_Size(N);
	char *memory = new char[N];//Ìàññèâ çíà÷åíèé êàæäîãî áàéòà
	string name_com;
	global_size = N;
	if ((global_size<100) || (global_size>10000)){
		cout << "Error! Input correct \n";
		return 1;

	}
	else{
		for (int j = 0; j < N - sys; j++)
			memory[j] = 'f';
		for (int j = (N - sys); j < N; j++)
			memory[j] = 'm';
	}


	while (1)//áåñêîíå÷íûé öèêë
	{
		cout << "\nInput name of command: ";
		cin >> name_com;
		if (name_com == "info"){
			size = N - (sys + res_adr);
			cout << "All Blocks:" << global_size << "; ";
			cout << "\tUser Blocks:" << block << "; ";
			cout << "\tBlocks,which Alloc sucsess:" << size << ";\n";
		}
		if (name_com == "alloc"){
			//int res_adr;
			cout << "Input value of the reserved block: ";
			cin >> res_adr;
			if ((res_adr == 0) || (res_adr>N-(sys))){
			//if ((res_adr == 0) || (res_adr >= ((N - sys) + 1))){
				cout << " - " << endl;
			}
			else
			{
				block++;//ñ÷åò÷èê ïîëüçîâàòåëüñêèõ áëîêîâ
				global_size--;
				for (int j = (N - (N - 0)); j < res_adr ; j++){
					memory[j] = 'u';
					
				}
				cout << " + " << block << endl;
			}
			//size = N - (sys + block);
			//block++;//ñ÷åò÷èê ïîëüçîâàòåëüñêèõ áëîêîâ
			//cout << " + " << block << endl;
		}
		if (name_com == "free"){
			if (block != 0){
				while (global_size != N){
					for (int j = 0; j < N; j++){
						if (memory[j] == 'u'){
							memory[j] = 'f';
						}
					}
					global_size++;
					block--;
					res_adr = 0;
					size = N - (sys + res_adr);
				}
				cout << " + " << endl;
			}
			else{
				cout << " - " << endl;
			}
		}
		if (name_com == "map"){
			cout << "Composition of dedicated memory\n";
			for (int j = 0; j < N; j++){
				cout << memory[j];
			}
			cout << "\n\n";
			cout << "1)m-system blocks;\n";
			cout << "2)u-user bloks;\n";
			cout << "3)f-free bloks;\n";
		}
		if (name_com == "exit") break;
	}
}
