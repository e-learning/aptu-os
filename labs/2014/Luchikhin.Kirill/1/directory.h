#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>

using std::string;

struct Directory{
    Directory(string const dirName):
        curDir(dirName)
    {}

    string getCurDir() const{
        return curDir;
    }

    void changeCurDir(string const newDirName);
    void dirLevelUp();
private:
    string curDir;
};

#endif // DIRECTORY_H
