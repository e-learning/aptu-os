#include "comminterp.h"

using namespace std;

string s_comm_args(string in_zn, int mode){
	string rez0, rez1;
	char flag=0;
	for (int i=0; i<strlen(in_zn.c_str()); i++){
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
	if (mode) return rez1;
	return rez0;
} 

int fLS(void){
	DIR *d = opendir(".");
	if(!d) return 1;
	
	struct dirent *f_dir;
	while ((f_dir=readdir(d))!=NULL)cout<< f_dir->d_name <<endl; 
	closedir(d);
	return 0;
}

int fKILL(string args){
	string arg1=s_comm_args(args, 0);
	string arg2=s_comm_args(s_comm_args(args, 1),0);
	if ((arg1=="") || (arg2=="")) return 1;
	return kill(atoi(arg1.c_str()),atoi(arg2.c_str()));
}
