#ifndef HELPER_H
#define HELPER_H

#include "Const.h"
#include "utility.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Root;
class Bitmap;
class FileRecord;
class Dir;


class Bitmap
{
    size_t bitmapCount_;

public:
    Bitmap(size_t bitmapCount) : bitmapCount_(bitmapCount)
    {}

    void initWriteSelf(Root & root);
    size_t get_nextfreeblock(Root & root);
    void set_blockBusy (size_t blockNumber, Root & root);
    void set_blockFree (size_t blockNumber, Root & root);
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

    File():
        name_(""), size_(0), time_(0), startblock_(LAST_BLOCK), buf_(NULL)
    {}

    File(std::string const & name, size_t size, size_t time, size_t startblock) :
        name_(name), size_(size), time_(time), startblock_(startblock), buf_(NULL)
    {
        //blocks_.push_back(startblock_);
    }

    File (File const & fr):
        name_(fr.name_), size_(fr.size_), time_(fr.time_), startblock_(fr.startblock_), blocks_(fr.blocks_), buf_(NULL)
    {
        if (fr.buf_ != NULL) {
            delete buf_;
            buf_ = new char[size_];
            std::copy(fr.buf_, fr.buf_ + fr.size_, buf_);
        }
    }

    ~File()
    { delete buf_; }

    size_t get_sizeInBlocks()
    { return blocks_.size(); }

    void readAboutSelf(std::fstream & fs);
    void readSelf(Root & root);
    size_t read_hostfile(std::string const & hostfile);
    void read_fsfile (Root & root, File & file);
    void write_hostfile (std::string const & hostfile);
    void writeAboutSelf(std::fstream & fs);
    void writeSelf(Root & root, Bitmap & bitmap);
    void deleteSelf(Root & root, Bitmap & bitmap);
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
    std::vector<File> filerecords_;
    std::vector<Dir> dirrecords_;

    Dir ():
        name_(""), size_(0), time_(0), startblock_(LAST_BLOCK)
    {}

    Dir(std::string const & name, size_t size, size_t time, size_t startblock):
        name_(name), size_(size), time_(time), startblock_(startblock)
    {
        //blocks_.push_back(startblock_);
    }

    Dir(Dir const & dir):
        name_(dir.name_), size_(dir.size_), time_(dir.time_), startblock_(dir.startblock_),
        blocks_(dir.blocks_), filerecords_(dir.filerecords_), dirrecords_(dir.dirrecords_)
    {}

    Dir & operator= (Dir const & dir)
    {
        name_ = dir.name_;
        size_ = dir.size_;
        time_ = dir.time_;
        startblock_ = dir.startblock_;
        filerecords_ = dir.filerecords_;
        dirrecords_ = dir.dirrecords_;

        return *this;
    }

    ~Dir()
    {}

    size_t get_recordsCount() const
    { return dirrecords_.size() + filerecords_.size(); }

    size_t get_sizeInBlocks()
    { return blocks_.size(); }

    size_t get_size();
    void readAboutSelf(std::fstream & fs);
    void readSelf(Root & root);
    void writeAboutSelf(std::fstream & fs);
    void writeSelf(Root & root, Bitmap & bitmap);
    void deleteSelf(Root & root, Bitmap & bitmap);

    void add_filerecord (File & fr);
    void add_dirrecord (Dir & dir);
    void copy_filerecord (Root & root, Bitmap & bitmap, File & fr);
    void copy_dirrecord (Root & root, Bitmap & bitmap, Dir & dir);
    size_t add_filerecord_by_name(std::string const & filename, Root & root, Bitmap & bitmap);
    size_t add_dirrecord_by_name(std::string const & dirname, Root & root, Bitmap & bitmap);
    void refresh_file (File & fr);
    void refresh_dir (Dir & dir);
    void remove_file(Root & root, Bitmap & bitmap, size_t filenumber);
    void remove_subdir(Root & root, Bitmap & bitmap, size_t dirnumber);
    size_t find_file_by_name (std::string const & filename);
    size_t find_dir_by_name (std::string const & dirname);
    Dir & get_subdir(size_t dirnumber);
    File & get_file (size_t filenumber);
    void create_subdir_by_name (Root & root, Bitmap & bitmap, std::string const & subdirName);
    Dir & get_lastdir();
    void printInfo();
};

class Root
{
    std::string root_;
    size_t freeBlocksCount_;
    size_t blockSize_;
    size_t bitmapCount_;
    size_t blocksCount_;
    Dir rootDir_;

    void initSelf();

public:

    Root (std::string root, size_t freeBlocksCount, size_t blockSize, size_t bitmapCount, size_t blocksCount, Dir rootDir);

    Root (std::string const & root): root_(root)
    { initSelf(); }

    ~Root()
    {}

    std::string const & get_root() const
    { return root_; }

    size_t get_freeBlockscount() const
    { return freeBlocksCount_; }

    size_t get_blockSize() const
    { return blockSize_; }

    size_t get_bitmapCount() const
    { return bitmapCount_; }

    size_t get_blocksCount() const
    { return blocksCount_; }

    size_t get_freeSpace() const
    { return freeBlocksCount_ * (blockSize_ - BYTES_FOR_BLOCKNUMBER); }

    size_t get_rootRecordStartBlock() const
    { return bitmapCount_ + 1; }

    Dir & get_rootDir ()
    { return rootDir_; }

    void refresh_freeBlocksCount (int diff);
    void writeSelf();
    void writeRootDir(Bitmap & bitmap);
};

#endif // HELPER_H
\













