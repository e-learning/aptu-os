#include <iostream>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fstream>

using namespace std;

void obrab_sig (int zn){
	printf ("\n Ctrl+c \n");
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



int main(void){

         void *lib;
        int (*fPS)(string *);
        int load_lib_flag=0;
       
        lib = dlopen("./libps.so", RTLD_LAZY);

        if (!lib) {
                cout<<"Не удалось подгрузить библиотеку :("<<endl;
                load_lib_flag = 1;
        }else{
                //вычисление адреса функции
                fPS = (int (*)(string *))dlsym(lib, "fPS");
                if (fPS==NULL) {
                        cout<<"Функция не найдена"<<endl;
                        load_lib_flag = 2;
                }
        }


        string command, comm, args, rez_fPS;
        int err_code;

        signal(SIGINT, obrab_sig);

        while(1){
                cout<<":";
                getline(cin,command);
                cout<<"-----------"<<endl;
                        
                err_code = -1;

                comm = s_comm_args(command, 0);
                args = s_comm_args(command, 1);

                if (comm=="exit") break;        

                if (comm=="ls") err_code = fLS();

                if (comm=="pwd") { err_code = 0; cout<<getenv("PWD")<<endl; } 

		if ((comm=="ps")&&(!load_lib_flag)) { err_code = fPS(&rez_fPS); cout<<rez_fPS; }

                if (comm=="kill") err_code = fKILL(args);

                if (comm=="run") {err_code = 0; system(args.c_str()); }


        }
        return 0;
}
