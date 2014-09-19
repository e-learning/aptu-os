#pragma once

#include <dirent.h>
#include <cerrno>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace utils {

    inline static int getdir(string dir, vector<string> &files) {
        DIR *dp;
        struct dirent *dirp;
        if ((dp = opendir(dir.c_str())) == NULL) {
            cout << "Error(" << errno << ") opening " << dir << endl;
            return errno;
        }

        while ((dirp = readdir(dp)) != NULL) {
            files.push_back(string(dirp->d_name));
        }
        closedir(dp);
        return 0;
    }

    inline static bool isNumber(string s) {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it)) ++it;
        return !s.empty() && it == s.end();
    }
}