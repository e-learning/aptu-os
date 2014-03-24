#include <iostream>
#include "comminterp.h"
#include <stdlib.h>
#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <fstream>

using namespace std;

void signal(int zn){
        printf("\rВы нажали сочетание Ctrl+c\n\r: ");
}

int fLS(void){
        DIR *d = opendir(".");
        if(!d) return 1;
       
        struct dirent *f_dir;
        while ((f_dir=readdir(d))!=NULL)cout<< f_dir->d_name <<endl;
        closedir(d);
        return 0;
}


int fPS(void){
        DIR *d=opendir("/proc");
        if (!d) return 1;

        struct dirent *f_dir;
        FILE *in_f;
        std::string f_name;
        char p_name[20];
        printf("  PID  NAME\n\r");
        while((f_dir=readdir(d))!=NULL){
                if ((f_dir->d_name[0] <= '9')&&(f_dir->d_name[0] >= '0')){
                        f_name = "/proc/" + string(f_dir->d_name) + "/comm";
                        in_f = fopen(f_name.c_str(), "r");
                        if (in_f == NULL) return 2;
                        fscanf(in_f, "%s", &p_name);
                        printf("%5s  %s\n\r", f_dir->d_name, p_name);
                        fclose(in_f);
                }
        }
        return 0;
}


int main(void){
       
        cout<<"Hello, "<<getenv("USERNAME")<<endl;

        string command, comm, args;
	char h[40];
        int err_code;
       
        signal(SIGINT, signal);

        while(1){
                cout<<":";
                getline(cin,command);
		memcpy(h,command.c_str(),40);
		comm=strtok(h," ");

                cout<<"_________"<<endl;
                       

      
                if (comm=="ls") err_code = fLS();

                if (comm=="pwd")  cout<<getenv("PWD")<<endl;

                if (comm=="ps") err_code = fPS();

                if (comm=="kill") {
			args=strtok(NULL," ");
			kill((atoi(args.c_str())),SIGKILL);
		}
		
		if (comm=="exit") break;        

                cout<<"_________"<<endl;

 
        }
       
        return 0;
}


