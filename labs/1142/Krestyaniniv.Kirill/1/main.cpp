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
                        f_name = "/proc/" + std::string(f_dir->d_name) + "/comm";
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

        string command, comm, args;
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

                if (comm=="ps") err_code = fPS();

                if (comm=="kill") err_code = fKILL(args);

                if (comm=="run") {err_code = 0; system(args.c_str()); }


        }
        return 0;
}
