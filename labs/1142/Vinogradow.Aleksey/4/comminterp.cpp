#include "comminterp.h"

using namespace std;

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
