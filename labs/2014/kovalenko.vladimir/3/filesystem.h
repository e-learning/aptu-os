#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "entries.h"
#include <vector>
#include <fstream>

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;

using std::ios;


class FileSystem {

public:
    FileSystem(const char *root);
    ~FileSystem();

    void format();
    void rm(const FileEntry &entry);
    void copy(const FileEntry &, FileEntry &);
    void move(const FileEntry &, FileEntry &);
    vector<FileEntry> list(const FileEntry & directory) const;

    FileEntry findEntry(const char *destination, bool doCreate = true, bool isDir = false);
    FileEntry findDir(const char *destination, bool create = true);






    ConfigInfo readConfig();
    void readData(FileEntry const &, std::ostream &);
    void writeData(FileEntry &, std::istream &);

    template <typename D = FileEntry>
    D readEntry(int id) const {
        D entry;
        ifstream block(getBlockById(id), ios::binary);
        block.read((char *) &entry, sizeof(D));
        if (block) {
            return entry;
        } else {
            throw "Cannot read file entry ";
        }
    }

    void writeEntry(const FileEntry &entry) {
        if (entry.id == 0) {
            rootDir = entry;
        }
        ofstream block(getBlockById(entry.id), ios::binary);
        block.write((char *) &entry, sizeof(FileEntry));
        if (!block) {
            throw "Cannot write file entry";
        }
        freeBlocks[entry.id] = true;
    }

    void checkInit(){
        if (!isInitialised) {
            throw "Not initialised yet!";
        }
    }

    const string getBlockById(const int id) const {
        return string(root) +"/"+ std::to_string(id);
    }

private:
    const char *root;
    const ConfigInfo config;
    const int blockSize;
    const bool isInitialised;
    FileEntry rootDir;
    vector<bool> freeBlocks;

    FileEntry initEntry(const string &name, bool isDir = true);
    void clearEntry(const FileEntry &fd);
    void rmEntry(int const);

    FileEntry findEntry(FileEntry &directory, const string &entryName, bool doCreate = true, bool isDirectory = false);
    FileEntry findDir(const vector<string> &path, bool create = true);

    void readBlockMap();
    void writeBlockMap();

    size_t getFirstAvailableBlock(const int start = 0) const {
        for (size_t i = start + 1; i < freeBlocks.size(); i++){
            if (!freeBlocks[i]) {
                return i;
            }
        }
        throw "Not enough space";
    }
};

#endif // FILESYSTEM_H
