#ifndef ENTRIES_H
#define ENTRIES_H

#include <time.h>
#include <iostream>
#include <vector>
#include <cstring>

struct FileEntry {
    static const int MAX_NAME_LENGTH = 10;

    int id;

    bool dir;

    int firstChild;

    int parentFile;

    int nextFile;

    int firstBlock;

    int previousFile;

    size_t nBlocks;

    char name[MAX_NAME_LENGTH];

    time_t time;
};

struct BlockEntry {
    int id;
    int next;
    size_t length;
};

struct ConfigInfo {
    int blockSize;
    int numberOfBlocks;
};

template<typename T>
std::ostream & operator<<(std::ostream &output, const std::vector<T> & v) {
    output << v.size() << " file(s)" << std::endl;
    for (auto it = v.begin(); it!=v.end(); ++it) {
        output << *it;
    }
    return output;
}

inline std::ostream & operator <<(std::ostream &output, const FileEntry &entry) {
    output << (entry.dir ? 'd' : 'f') << "\t '" << entry.name << "'\t";

    if  (entry.dir) {
        output << std::endl;
    } else {
        output << entry.nBlocks << " blocks";
        output << '\t' << ctime(&entry.time);
    }
    return output;
}

#endif //
