#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <cctype>
#include <fstream>
#include "ps.h"

extern "C" void ps(){
    DIR *dir;
    struct dirent *entry;
    dir = opendir("/proc");
    if (!dir) {
        std::cerr << "Crash open dir(ps)" << std::endl;
        exit(1);
    };
    std::ifstream file_in;
    while ( (entry = readdir(dir)) != NULL) {
        if(isdigit(entry->d_name[0])){
            std::string way;
            std::string way_val(entry->d_name);
            std::string info = "";

            way = "/proc/" + way_val + "/status";
            file_in.open(way.c_str());
            for(int i = 0; i < 3 ;i++){
                std::string str= "";
                getline(file_in, str);
                info = info + str + " ";
            }
            file_in.close();
            std::cout << info << std::endl;
        }
    }
    closedir(dir);
}
