#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#define EXIST_FLAG 1
#define FLAG_DIR 2

#include "fileentry.h"
#include "headers.h"
#include "block.h"
#include "block_pack.h"



#include <string>
#include <map>
using std::string;
using std::map;

class File_system {
public:
    File_system(const char*  rootPath);
    ~File_system();

    bool isReady() { return ready; }

    bool init();
    bool format();
    bool load();
    bool save();

    bool fileInfo(const string& path);

    bool import_file(const string& src, const string& dst);
    bool export_file(const string& src, const string& dst);

    bool remove_file(const string& path, bool recursive = false);
    bool remove(const string& path, bool recursive = false);

    bool move(const string& src, const string& dst);
    bool copy(const string& src, const string& dst);

    bool flag;

    File_entry* create_file(const string& path, bool alloc = false);
    File_entry* forced_create_file(const string& path, bool alloc = false);
    File_entry* create_dir(const string& path);
    File_entry* forced_create_dir(const string& path);

    Block*  get_block(size_t n) {
        return blocks->getBlock(n);
    }

    void print_bitmap();
    size_t freeSpace() const;

private:
    bool ready;
    int Size_of_superblock, N, super_block_length;
    string path;
    Super_block_info super_block;
    File_entry root;

    Block_pack*  blocks;

    string config_path(const char*  rootPath);

    bool isUsedBlock(int n) const;
    void reserveBlock(int n);
    int allocateBlock();
    void free_block(int n);

    void writeSuperBlock();
    string getBlockPath(int n);
    void stretchBlock(FILE*  block);

    File_entry*  findFile(const string& path, bool test = false);
    bool copyFile(File_entry*  src, const string& dst);
    bool copyDir(Block*  src, const string& dst);
    bool directory_info(File_entry*  dir);
};



#endif // FILESYSTEM_H
