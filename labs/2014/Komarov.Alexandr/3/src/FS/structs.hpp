#ifndef STRUCTS_H
#define STRUCTS_H

struct HeaderForBlock {
    int next;
};

struct HeaderForDirectory {
    size_t fileCount;
};

struct InfoForSuperBlock {
    int blockSize;
    int blockCount;
    int blockNext;
    int blockRoot;
    int mapSize;
    char *map;
};

#endif //STRUCTS_H
