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



#define NAME_BOUND 10

struct File_entry {
    char attr;
    char filename[NAME_BOUND];
    int size;
    int blocks;
    int first_block;
    time_t last_modified;

    File_entry() : attr(0), size(0), blocks(0), first_block(0), last_modified(0) {
        memset(filename, 0, NAME_BOUND*sizeof(char));
    }

    std::string name() const {
        if(filename[NAME_BOUND-1]) return std::string(filename, NAME_BOUND);
        return std::string(filename);
    }

    void copy(const File_entry *other) {
        attr = other->attr;
        size = other->size;
        blocks = other->blocks;
        first_block = other->first_block;
        last_modified = other->last_modified;
        strncpy(filename, other->filename, NAME_BOUND);
    }

    std::string info() const {
        std::stringstream str;
        str << name() << "\t";
        str <<"blocks:"<< blocks <<"\t";
        str <<"last modified: "<< ctime(&last_modified);
        return str.str();
    }
};

#endif //FILEENTRY_H
