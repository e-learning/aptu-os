#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <dlfcn.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void sign(int i){
        if (i==SIGINT) cout<<"перехвачен сигнал ctrl+c"<<endl;
}

int main(void){

        string command;
        int c;

        int (*PS)(string *);
        void *l;
        l = dlopen("./libps.so", RTLD_LAZY);
        if (!l) {cout<<"Error load lib!"<<endl; return 1;}

        PS = (int (*)(string *))dlsym(l, "PS");
        if (PS == NULL) {cout<<"Функция не найдена"<<endl; return 2;}

    signal(SIGINT, sign);


        while (1){
                cout<<">";
                getline(cin,command);
                c=0;
                if (command == "exit") break;

                if (command == "ls") c=1;

                if (command == "pwd") c=2;

                if (command == "ps") c=3;

                if (command == "kill") c=4;

                switch (c){

                case 1:{
                        DIR *d = opendir(".");
                        if(d){
                                struct dirent *dir;
                                while ((dir=readdir(d))!=NULL)cout<< dir->d_name <<endl; 
                                closedir(d);
                        }
                        break;
                }

                case 2: {
                        cout<<getenv("PWD")<<endl;
                         break;
                }

                case 3:{     
                        PS(&command);
                        break;
                }

                case 4:{
                        cout<<"chell: KILL:"<<endl<<"enter PID:"<<endl;
                        int pid, sig;
                        cin>>pid;
                        cout<<"enter signal:"<<endl;
                        cin>>sig;
                        kill(pid, sig);
                        break;
                }

                default:
                        system(command.c_str());

                }
        }
        dlclose(l);
        return 0;
}