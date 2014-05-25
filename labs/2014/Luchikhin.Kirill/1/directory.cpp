#include "directory.h"

#include <unistd.h>
#include <dirent.h>

void Directory::changeCurDir(string const newDirName){
    DIR *dir = opendir(newDirName.c_str());
    if(!dir) return;
    chdir(newDirName.c_str());
    curDir = get_current_dir_name();

}

void Directory::dirLevelUp(){
    curDir = curDir.substr(0, curDir.find_last_of('/'));
    chdir(curDir.c_str());
}
