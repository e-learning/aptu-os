#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include <signal.h>


using std::cout;
using std::cin;
using std::vector;
using std::getline;
using std::string;
using std::endl;
using std::istringstream;
using std::ifstream;

vector <string> getParams(string const & );
bool isNum(string const &);
void lsFunc();
void pwdFunc();
void psFunc();
void killFunc(string const & );

int main()
{
    string inCommand;
    while (true)
    {
        cout << "> ";
        getline(cin, inCommand);

        string command = getParams(inCommand)[0];

        if(strcmp(command.c_str(), "ls") == 0)
            lsFunc();

        else if(strcmp(command.c_str(), "pwd") == 0)
            pwdFunc();

        else if(strcmp(command.c_str(), "ps") == 0)
            psFunc();

        else if(strcmp(command.c_str(), "kill") == 0)
            killFunc(inCommand);

        else if(strcmp(command.c_str(), "exit") == 0)
            return false;

        else system(inCommand.c_str());
    }

    return 0;
}

vector <string> getParams(string const & inStr)
{
    istringstream iss(inStr);
    string sub;
    vector <string> allParts;
    while (iss >> sub)
    {
        allParts.push_back(sub);
    }
    return allParts;
}

bool isNum(string const &line)
{
    char* p;
    strtol(line.c_str(), &p, 10);
    return *p == 0;
}

void lsFunc()
{
    DIR *dp;
    dirent *d;
    dp = opendir(".");

    while((d = readdir(dp)) != NULL)
    {
        if(!strcmp(d->d_name,".") || !strcmp(d->d_name,".."))
            continue;
        cout << d->d_name << "  ";
    }
    cout << endl;
    closedir(dp);
}

void pwdFunc()
{
    char path[256];
    if (getcwd(path, sizeof(path)) != NULL)
        cout << path << endl;
}

void psFunc()
{
    DIR *dp;
    dirent *d;
    dp = opendir("/proc");
    while((d = readdir(dp)) != NULL)
    {
        if(!strcmp(d->d_name,".") || !strcmp(d->d_name,".."))
            continue;

        if (isNum(string(d->d_name)))
        {
            string line = "";
            string fileName = "/proc/" + string(d->d_name) + "/comm";


            ifstream myfile(fileName.c_str());
            if (myfile.is_open())
            {
                getline (myfile,line);
                myfile.close();
            }
            cout << d ->d_name << " " << line << endl;
        }

    }
    closedir(dp);

}

void killFunc(string const & inCommand)
{
    vector <string> args = getParams(inCommand);
    int rez = 0;
    if (args.size() > 2)
    {
        if ((isNum(args[1])) && (isNum(args[2])))
        {
            rez = kill(atoi(args[1].c_str()), atoi(args[2].c_str()));
            if (rez == -1) cout << "fails to send a signal" << endl;
        }
        else
            cout << "params should be numeric" << endl;
    }
    else
        cout <<  "you should enter more arguments to command" << endl;
}

