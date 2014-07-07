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
	int system_adr = 0;
	int r = 0;
	int z = 0;
	int p=0;
	int l=0;
	int res_adr = 0;
	int user_blocks = 0;//÷èñëî âûäåëåííûõ ïîëüçîâàòåëþ áëîêîâ
	int system_blocks = 0;//÷èñëî ñëóæåáíûõ áëîêîâ
	int sys = 0;//Ðåçåðâèðóåì ïîñëåäíèå 16 áàéò ïîä ñëóæåáíûå äàííûå
	int N = 0;//Ðàçìåð âûäåëÿåìîé ïàìÿòè
	int global_size = 0, block = 0, size = 0;
	N = Memory_Size(N);
	char *memory = new char[N];//Ìàññèâ çíà÷åíèé êàæäîãî áàéòà
	string name_com;
	global_size = N;
	if ((global_size<100) || (global_size>10000)){
		cout << "Error! Input correct \n";
		return(0);

	}
	else{
		for (int j = 0; j < N; j++)
			memory[j] = 'f';
	}


	while (1)//áåñêîíå÷íûé öèêë
	{
		if (user_blocks == 0){
			size = global_size;
		}
		else{
			size = global_size - (user_blocks + system_blocks);
		}
		cout << "\nInput name of command: ";
		cin >> name_com;
		if (name_com == "info"){
			cout << "User blocks:" << block << "; ";
			cout << "\tSize user memory blocks:" << user_blocks << "; ";
			cout << "\tBlocks,which Alloc sucsess:" << size << ";\n";
		}
		
		if (name_com == "alloc"){
			cin >> res_adr;
			
			if ((res_adr==0)||(res_adr>=size)){
				cout << " - " << endl;
			}
			else
			{
				user_blocks += res_adr;
				block++;//÷èñëî ðàç âûäåëåíèÿ áëîêîâ
				for (int j = l; j < N; j++){
					if ((memory[j] == 'f')&&(p==0)){
							memory[j] = 'm';
							system_adr = j;
							p++;
							system_blocks++;
						}
					else{
						for (int s = l + 1; s < (N - (N - (res_adr + (z+1)))); s++){
							memory[s] = 'u';
							l = s;
						}
					}
				}
				l = l + 1;
				z = l;
				p = 0;
				cout << " + " << system_adr+1 << endl;
			}
		}
		
		if (name_com == "free"){
			cin >> system_adr;			
			if ((memory[system_adr-1] != 'm')){
				cout << " - " << endl;
			}
			else{
				int j;
				j = system_adr;
				while ( (memory[j] == 'u')){
					if ((memory[j - 1] == 'm') && (r == 0)){
						memory[j - 1] = 'f';
						memory[j] = 'f';
						r++;
					}
					else{
						r++;
						memory[j] = 'f';
					}
					j++;
					user_blocks--;
				}
				block--;
				r = 0;
				cout << " + " << endl;
			}
		}
		if (name_com == "map"){
			cout << "\nComposition of dedicated memory\n";
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
