#include "command_handler.h"
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <errno.h>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

using std::ifstream;


void ps  (vector<string> const & command_tokens){
	DIR *dirp;
    struct dirent *dp;

    if ((dirp = opendir("/proc/")) == NULL) {
        cerr << "couldn't open '.'" << endl;
        return;
    }
	cout << "PID\tCMD" << endl;
    do {
        errno = 0;
        if ((dp = readdir(dirp)) != NULL) {
			string filename = string("/proc/") + dp->d_name +"/status"; 
			ifstream status(filename.c_str());
			if (status.is_open()){
				string pid, name;
 
				string line;
				while (getline(status, line)){
					if (line.substr(0, 4)=="Name"){
						name = line.substr(6);
					}else if (line.substr(0,3) == "Pid"){
						pid = line.substr(5);
					}
				}				
				cout << pid << '\t' << name << endl;
			}
        }
    } while (dp != NULL);

    if (errno != 0)
        cerr << "error reading directory" << endl;
    closedir(dirp);
}
