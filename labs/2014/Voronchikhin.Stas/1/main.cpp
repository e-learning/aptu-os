

#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <stdlib.h>
#include <fstream>
#include <signal.h>
#include <unistd.h>


using namespace std;

vector<string> parseCommand(string input);
void runCommand (vector<string>& cmd,string& src);
string getNextComand (string& s,size_t& i);
bool ch (vector<string>& cmd,string comand);

void ls();
void pwd();
void ps();
bool isdigit(string& s);
void shellKill(vector<string>& cmd);

int main (){
    string input;
    while (true){
        cout<<"shell#";
        getline(cin,input);
        if(input=="exit"){
            break;
        }else{
            vector<string> cmd = parseCommand(input);
            if(cmd.size()>0){
//                for(vector<string>::iterator it = cmd.begin();it!=cmd.end();++it){
//                    cout<<*it<<" ";
//                }
//                cout<<"\n";
                runCommand(cmd,input);
          }
        }

    }
return 0;
}

void runCommand (vector<string>& cmd,string& src){
    if(ch(cmd,"ls")){
        ls();
    }else if (ch(cmd,"pwd"))
    {
        pwd();
    }
    else if (ch(cmd,"ps"))
        {
            ps();
        }
    else{
        system(src.c_str());
    }
}

void ls(){
    DIR *dir;
    dirent *file;

    if((dir = opendir("."))!= NULL){
    while ((file = readdir (dir)) != NULL)
    {
        std::cout << file->d_name << "\n";
    }

    }
    else{
        cout<<"couldnt opent dir"<<"\n";
    }
    closedir (dir);
}

void pwd(){
 char cCurrentPath[FILENAME_MAX];

 if (!getcwd(cCurrentPath, sizeof(cCurrentPath)))
     {

     cout<<"smth goes wrong...\n";
     }
printf ("%s \n", cCurrentPath);
}

void ps(){
    DIR *dir;
    dirent *file;

    ifstream comm;

    cout<<"PID \t NAME\n";
    if((dir = opendir("/proc"))!= NULL){
    while ((file = readdir (dir)) != NULL)
    {
        string folder=string(file->d_name);
        if(isdigit(folder))
        {
            cout<<folder<<"\t";
            string comPath("/proc/"+folder+"/comm");
            comm.open( comPath.c_str() );

            string name;
            comm>>name;
            comm.close();

            cout<<name<<"\n";
        }

    }
    }
    else{
        cout<<"couldnt opent dir"<<"\n";
    }
    closedir (dir);

}

void shellKill(vector<string>& cmd){
    if(cmd.size()<3){
        cout<<"u need pid and signal to kill the process! \n";
        return;
    }else{
        if (isdigit(cmd[1]) && isdigit(cmd[2])){
            pid_t pid = atoi(cmd[1].c_str());
            int sig=atoi(cmd[2].c_str());
            if(kill(pid,sig)==-1){
                cout<<"kill failed \n";
            }

        }
            else{cout<<"your pid or signal is not number \n";
        return;}
    }
}


vector<string> parseCommand(string input){
vector<string> res;
size_t l = input.length();
size_t i=0;
    while(i<l){
        string cmd=getNextComand(input,i);
        res.push_back(cmd);
    }

    return res;
}

string getNextComand (string& s,size_t& i){
   size_t l = s.length();
   while((s[i]==' ' || s[i]=='\t') && i<l)++i;
    string buffer="";

    while(s[i]!=' ' && s[i]!='\t' && i<l){
        buffer+=s[i];
        ++i;
    }
    return buffer;
}
bool ch (vector<string>& cmd,string comand){
    return cmd[0]==comand;
}

bool isdigit(string& s){
    for (string::iterator it=s.begin();it!=s.end();++it){
        if(!isdigit(*it))return false;
    }
    return true;
}
