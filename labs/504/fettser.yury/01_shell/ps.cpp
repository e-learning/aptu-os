#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <sys/param.h>
#include <dlfcn.h>

using namespace std;

bool IsNumerics(const char *str)
{
    for (; *str; str++)
        if (*str < '0' || *str > '9')
            return 0;
    return 1;
}

extern "C" int ps(char **args, int argc)
{
    (void)args;
    if (argc != 1)
        cout << "our ps doesn't have any parameters" << endl;
    else
    {
        DIR *dir;

        string procdir = "/proc/";

        if ((dir = opendir(procdir.c_str())) == 0)
            cout << "Couldn't read proc directory" << endl;
        else {
            dirent *pid;
            string commandLine;
            while ((pid = readdir(dir)) != 0) {
                if (pid->d_type == DT_DIR && IsNumerics(pid->d_name)) {
                    string cmdFilePath = procdir + string(pid->d_name) + "/comm";
                    ifstream cmdFile(cmdFilePath.c_str());

                    if (cmdFile.is_open()) {
                        getline(cmdFile, commandLine);
                        cout << pid->d_name << ": " << commandLine << endl;
                    }
                    cmdFile.close();
                }
            }
            closedir(dir);
        }
    }
    return 0;
}

