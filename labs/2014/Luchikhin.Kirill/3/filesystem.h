#ifndef FILESYSTEM_H
#define FILESYSTEM_H

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

#define FA_USED 1
#define FA_DIR 2

struct SuperBlock {
    int bsize;
    int bcount;
    int bnext;
    int broot;
    int mapSize;
    char *map;
};

struct BlockHeader {
    int next;
};

struct DirHeader {
    size_t fileCount;
};

struct FileEntry {
    char attr;
    char filename[MAX_NAME_LENGTH];
    int size;
    int blocks;
    int firstBlock;
    time_t modTime;

    FileEntry() : attr(0), size(0), blocks(0), firstBlock(0), modTime(0) {
        memset(filename, 0, MAX_NAME_LENGTH*sizeof(char));
    }

    std::string name() const {
        if(filename[MAX_NAME_LENGTH-1]) return std::string(filename, MAX_NAME_LENGTH);
        return std::string(filename);
    }

    void copy(const FileEntry *other) {
        attr = other->attr;
        size = other->size;
        blocks = other->blocks;
        firstBlock = other->firstBlock;
        modTime = other->modTime;
        strncpy(filename, other->filename, MAX_NAME_LENGTH);
    }

    std::string info() const {
        std::stringstream str;
        str << "File name: " << name() << "\n";
        str << "Size (bytes): " << size << "\n";
        str << "Size (blocks): " << blocks << "\n";
        str << "Modified: " << ctime(&modTime);
        return str.str();
    }
};

//------------------------------------------------------------------

class Block {
public:
    Block(size_t i, size_t s);
    Block(size_t i, size_t s, const char *bn);
    ~Block();

    void read(const char *bn);
    void save(const char *bn);

    char *data() { return bdata; }
    size_t size() const { return bsize; }
    size_t index() const { return bindex; }
    BlockHeader *header() { return &bheader; }

private:
    BlockHeader bheader;
    size_t bindex;
    size_t bsize;
    char *bdata;
};

//------------------------------------------------------------------

class BlockBuffer {
public:
    BlockBuffer(size_t s, const std::string &p);
    ~BlockBuffer();

    Block *getBlock(size_t n);
    void save();

private:
    size_t bsize;
    std::string path;
    std::map<size_t, Block> blocks;
};

//------------------------------------------------------------------

class FileSystem {
public:
    FileSystem(const char *rootPath);
    ~FileSystem();

    bool isReady() { return ready; }

    bool init();
    bool format();
    bool load();
    bool save();

    bool viewDir(const std::string &path);
    bool fileInfo(const std::string &path);

    bool importFile(const std::string &src, const std::string &dst);
    bool exportFile(const std::string &src, const std::string &dst);

    bool removeFile(const std::string &path, bool recursive = false);
    bool remove(const std::string &path, bool recursive = false);

    bool move(const std::string &src, const std::string &dst);
    bool copy(const std::string &src, const std::string &dst);

    FileEntry *createFile(const std::string &path, bool alloc = false);
    FileEntry *forcedCreateFile(const std::string &path, bool alloc = false);
    FileEntry *createDir(const std::string &path);
    FileEntry *forcedCreateDir(const std::string &path);

    Block *getBlock(size_t n) {
        return blocks->getBlock(n);
//        if(n < blocks.size()) return &blocks[n];
//        return 0;
    }

    void printMap();
    size_t freeSpace() const;

private:
    bool ready;
    int S, N, sBlockLength;
    std::string path;
    SuperBlock sBlock;
    FileEntry root;
    //std::vector<Block> blocks;
    BlockBuffer *blocks;

    std::string configPath(const char *rootPath);

    bool isUsedBlock(int n) const;
    void reserveBlock(int n);
    int allocateBlock();
    void freeBlock(int n);

    void writeSuperBlock();
    std::string getBlockPath(int n);
    void stretchBlock(FILE *block);

    FileEntry *findFile(const std::string &path, bool test = false);
    bool copyFile(FileEntry *src, const std::string &dst);
    bool copyDir(Block *src, const std::string &dst);
    bool viewDir(FileEntry *dir);

};

//------------------------------------------------------------------

class DirIterator {
public:
    DirIterator(FileSystem *fsys, Block *b, bool skipEmpty = true);

    FileEntry *current();
    FileEntry *next();
    Block *currentBlock();
    DirHeader *dirHeader();
    bool atBorder() const;
    bool hasNext() const;

private:
    Block *curBlock;
    FileEntry *curFE;
    DirHeader *dh;
    FileSystem *fs;
    bool skip;
    size_t pos;
};

#endif // FILESYSTEM_H
