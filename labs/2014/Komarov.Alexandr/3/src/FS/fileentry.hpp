#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>



#define MAX_NAME_LENGTH 10

struct EntryForFile {
    char attr;
    char filename[MAX_NAME_LENGTH];
    int size;
    int blocks;
    int firstBlock;
    time_t modTime;

    EntryForFile() : attr(0), size(0), blocks(0), firstBlock(0), modTime(0) {
        memset(filename, 0, MAX_NAME_LENGTH*sizeof(char));
    }

    std::string name() const {
        if(filename[MAX_NAME_LENGTH-1]) return std::string(filename, MAX_NAME_LENGTH);
        return std::string(filename);
    }

    void copy(const EntryForFile *other) {
        attr = other->attr;
        size = other->size;
        blocks = other->blocks;
        firstBlock = other->firstBlock;
        modTime = other->modTime;
        strncpy(filename, other->filename, MAX_NAME_LENGTH);
    }

    std::string info() const {
        std::stringstream str;
        str << name() << "\t";
        str << blocks << " blocks \t";
        str << ctime(&modTime);
        return str.str();
    }
};

#endif //FILEENTRY_H
