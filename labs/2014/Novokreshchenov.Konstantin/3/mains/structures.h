#ifndef HELPER_H
#define HELPER_H

#include "Const.h"
#include "utility.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>

class Root;
class Bitmap;
class FileRecord;
class Dir;

typedef std::vector<size_t>::iterator BlockIt;

class Bitmap
{
    size_t bitmapCount_;
    size_t freeBlocksCount_;
    size_t blocksCount_;
    char* bitmapBuf_;


public:
    Bitmap ();
    Bitmap (Root * root);
    Bitmap (Bitmap const & bitmap);
    Bitmap & operator= (Bitmap const & bitmap);
    ~Bitmap();

    void initSelf(Root * root);
    void readSelf(Root * root);
    void writeSelf(Root * root);
    size_t get_nextFreeBlock();
    size_t get_freeBlocksCount();
    void set_blockFree(size_t blockNumber);
    void set_blockBusy(size_t blockNumber);
    bool hasFreeBlocksCount(size_t requiredBlocksCount);
};

class File
{
public:
    std::string name_;
    size_t size_;
    size_t time_;
    size_t startblock_;
    std::vector<size_t> blocks_;
    char* buf_;

    File();
    File(std::string const & name, size_t size, size_t time, size_t startblock);
    File (File const & fr);
    File & operator= (File const & fr);
    ~File();

    size_t get_size();
    size_t get_sizeInBlocks();
    void readAboutSelf(std::fstream & fs);
    void readSelf(Root * root);
    void read_hostfile(std::string const & hostfile);
    void read_fsfile (File * file);
	void clearBuf();
    void write_hostfile (std::string const & hostfile);
    void writeAboutSelf(std::fstream & fs);
    void writeSelf(Root * root, Bitmap * bitmap);
	size_t get_nextBlockForWrite(BlockIt & it, Bitmap * bitmap);
    void deleteSelf(Root * root, Bitmap * bitmap);
    void printInfo();

};

class Dir
{
public:
    std::string name_;
    size_t size_;
    size_t time_;
    size_t startblock_;
    std::vector<size_t> blocks_;
    std::vector<File*> filerecords_;
    std::vector<Dir*> dirrecords_;

    Dir ();
    Dir(std::string const & name, size_t size, size_t time, size_t startblock);

    Dir(Dir const & dir);

    Dir & operator= (Dir const & dir);

    ~Dir();

    size_t get_recordsCount() const;
    size_t get_sizeInBlocks();

    size_t get_size();
    void readAboutSelf(std::fstream & fs);
    void readSelf(Root * root);
    void writeAboutSelf(std::fstream & fs);
    void writeSelf(Root * root, Bitmap * bitmap);
    size_t get_nextBlockForWrite(BlockIt & it, Bitmap * bitmap);
    void deleteSelf(Root * root, Bitmap * bitmap);

    void add_filerecord (File * fr);
    void add_dirrecord (Dir * dir);
    void copy_filerecord (Bitmap * bitmap, File * fr);
    void copy_dirrecord (Bitmap * bitmap, Dir * dir);
    void remove_file(std::string const & filename);
    void remove_subdir(std::string const & dirname);
    File* find_file_by_name (std::string const & filename);
    Dir* find_dir_by_name (std::string const & dirname);
    void printInfo();
	void copy_records(Dir * srcDir);
	void copy_newrecords(Root * root, Bitmap * bitmap, Dir * srcDir);
};

class Root
{
    std::string rootpath_;
    size_t freeBlocksCount_;
    size_t blockSize_;
    size_t bitmapCount_;
    size_t blocksCount_;
    Dir* rootDir_;

    void initSelf();

public:

    Root (std::string root,
          size_t freeBlocksCount,
          size_t blockSize,
          size_t bitmapCount,
          size_t blocksCount,
          Dir* rootDir);

    Root (std::string const & rootpath);
    ~Root();

    std::string const & get_rootpath() const;
    size_t get_freeBlocksCount() const;
    size_t get_blockSize() const;
    size_t get_bitmapCount() const;
    size_t get_blocksCount() const;
    size_t get_freeSpace() const;
    size_t get_rootRecordStartBlock() const;
    Dir * get_rootDir ();
    void refresh_freeBlocksCount (size_t freeBlocksCount);
    void writeSelf();
    void writeRootDir(Bitmap & bitmap);
};

#endif // HELPER_H
\













