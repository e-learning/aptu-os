#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "fileentry.hpp"
#include "structs.hpp"

#define FLAG 1
#define FLAG_DIR 2

/////////////

class Block {
public:
    Block(size_t i, size_t s);
    Block(size_t i, size_t s, const char *bn);
    ~Block();

    void read(const char *bn);
    void save(const char *bn);

    char *data() { return blockData; }
    size_t size() const { return blockSize; }
    size_t index() const { return blockIndex; }
    HeaderForBlock *header() { return &blockHeader; }

private:
    HeaderForBlock blockHeader;
    size_t blockIndex;
    size_t blockSize;
    char *blockData;
};

/////////////

class BlockContainer {
public:
    BlockContainer(size_t s, const std::string &p);
    ~BlockContainer();

    Block *getBlock(size_t n);
    void save();

private:
    size_t blockSize;
    std::string path;
    std::map<size_t, Block> blocks;
};

/////////////

class FileSystem {
public:
    FileSystem(const char *rootPath);
    ~FileSystem();

    bool isReady() { return ready; }

    bool init();
    bool format();
    bool load();
    bool save();

    bool fileInfo(const std::string &path);

    bool importFile(const std::string &src, const std::string &dst);
    bool exportFile(const std::string &src, const std::string &dst);

    bool removeFile(const std::string &path, bool recursive = false);
    bool remove(const std::string &path, bool recursive = false);

    bool move(const std::string &src, const std::string &dst);
    bool copy(const std::string &src, const std::string &dst);

    bool flag;

    EntryForFile *createFile(const std::string &path, bool alloc = false);
    EntryForFile *forcedCreateFile(const std::string &path, bool alloc = false);
    EntryForFile *createDir(const std::string &path);
    EntryForFile *forcedCreateDir(const std::string &path);

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
    InfoForSuperBlock sBlock;
    EntryForFile root;
    //std::vector<Block> blocks;
    BlockContainer *blocks;

    std::string configPath(const char *rootPath);

    bool isUsedBlock(int n) const;
    void reserveBlock(int n);
    int allocateBlock();
    void freeBlock(int n);

    void writeSuperBlock();
    std::string getBlockPath(int n);
    void stretchBlock(FILE *block);

    EntryForFile *findFile(const std::string &path, bool test = false);
    bool copyFile(EntryForFile *src, const std::string &dst);
    bool copyDir(Block *src, const std::string &dst);
    bool directoryInfo(EntryForFile *dir);

};

/////////////

class DirIterator {
public:
    DirIterator(FileSystem *fsys, Block *b, bool skipEmpty = true);

    EntryForFile *current();
    EntryForFile *next();
    Block *currentBlock();
    HeaderForDirectory *dirHeader();
    bool atBorder() const;
    bool hasNext() const;

private:
    Block *curBlock;
    EntryForFile *curFE;
    HeaderForDirectory *dh;
    FileSystem *fs;
    bool skip;
    size_t pos;
};

#endif // FILESYSTEM_H
