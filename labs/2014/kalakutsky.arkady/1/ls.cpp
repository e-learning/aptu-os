#include <dirent.h>
#include <errno.h>
#include <iostream>

#include "command_handler.h"
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

void ls  (vector<string> const & command_tokens){
	DIR *dirp;
    struct dirent *dp;

    if ((dirp = opendir(".")) == NULL) {
        cerr << "couldn't open '.'" << endl;
        return;
    }

    do {
        errno = 0;
        if ((dp = readdir(dirp)) != NULL) {
            cout <<  dp->d_name << endl;
        }
    } while (dp != NULL);

    if (errno != 0)
        cerr << "error reading directory" << endl;
    closedir(dirp);
}
