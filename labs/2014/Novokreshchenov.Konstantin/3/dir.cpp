#include "Const.h"
#include "structures.h"
#include "converter.h"
#include "reader.h"
#include "writer.h"
#include "utility.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>


/*==Dir==*/
void Dir::readAboutSelf(std::fstream & fs)
{
    read_number_from_bytes(fs, BYTES_FOR_ISDIR);
    name_ = read_string_from_bytes(fs, BYTES_FOR_NAME);
    size_ = read_number_from_bytes(fs, BYTES_FOR_FILESIZE);
    time_ = read_number_from_bytes(fs, BYTES_FOR_TIME);
    startblock_ = read_number_from_bytes(fs, BYTES_FOR_BLOCKNUMBER);
}

void Dir::readSelf(Root & root)
{
    size_t nextblockNumber = startblock_;

    while (nextblockNumber != LAST_BLOCK) {
        blocks_.push_back(nextblockNumber);
        std::fstream dirfs ((root.get_root() + "/" + number_to_string(nextblockNumber)).c_str(),
                            std::fstream::in | std::fstream::out | std::fstream::binary);
        size_t recordsCount = read_number_from_bytes(dirfs, BYTES_FOR_RECORDS_COUNT);
        for (size_t i = 0; i < recordsCount; ++i) {
            size_t isdir = read_number_from_bytes(dirfs, BYTES_FOR_ISDIR);
            dirfs.seekg((int)BYTES_FOR_ISDIR * (-1), dirfs.cur);
            if (isdir == 0) {
                File file;
                file.readAboutSelf(dirfs);
                file.readSelf(root);
                filerecords_.push_back(file);
            }
            else {
                Dir dir;
                dir.readAboutSelf(dirfs);
                dir.readSelf(root);
                dirrecords_.push_back(dir);
            }
        }
        nextblockNumber = read_number_from_bytes(dirfs, BYTES_FOR_BLOCKNUMBER);
        if (nextblockNumber != LAST_BLOCK) {
            //blocks_.push_back(nextblockNumber);
        }
        dirfs.close();
    }
}

void Dir::writeAboutSelf(std::fstream & fs)
{
    write_number_to_bytes(fs, BYTES_FOR_ISDIR, 1);
    write_string_to_bytes (fs, BYTES_FOR_NAME, name_);
    write_number_to_bytes(fs, BYTES_FOR_FILESIZE, size_);
    write_number_to_bytes(fs, BYTES_FOR_TIME, time_);
    write_number_to_bytes(fs, BYTES_FOR_BLOCKNUMBER, startblock_);
}

void Dir::writeSelf(Root & root, Bitmap & bitmap)
{
    std::fstream blockfs ((root.get_root() + "/" + number_to_string(startblock_)).c_str(),
                          std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    bitmap.set_blockBusy(startblock_, root);

    std::vector<size_t>::iterator myblock = blocks_.begin();
    size_t records_count = 0;
    size_t metaInfo = BYTES_FOR_RECORDS_COUNT + BYTES_FOR_BLOCKNUMBER;
    write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT, 0); //фиктивное количество
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        if (root.get_blockSize() - records_count * BYTES_FOR_RECORD - metaInfo < BYTES_FOR_RECORD) {
            size_t curpos = blockfs.tellg();
            blockfs.seekg(0, blockfs.beg);
            write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT, records_count);
            blockfs.seekg(curpos, blockfs.beg);

            size_t nextblockfree = 0;
            if (myblock + 1 != blocks_.end()) {
                myblock += 1;
                nextblockfree = *myblock;
            }
            else {
                nextblockfree = bitmap.get_nextfreeblock(root);
            }
            write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, nextblockfree);
            blockfs.close();
            bitmap.set_blockBusy(nextblockfree, root);
            blockfs.open((root.get_root() + "/" + number_to_string(nextblockfree)).c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
            write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT, 0);
            records_count = 0;
        }
        else {
            dirrecords_[i].writeAboutSelf(blockfs);
            ++records_count;
        }
    }

    for (size_t i = 0; i < filerecords_.size(); ++i) {
        if (root.get_blockSize() - records_count * BYTES_FOR_RECORD - metaInfo < BYTES_FOR_RECORD) {
            size_t curpos = blockfs.tellg();
            blockfs.seekg(0, blockfs.beg);
            write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT, records_count);
            blockfs.seekg(curpos, blockfs.beg);

            size_t nextblockfree = 0;
            if (myblock + 1 != blocks_.end()) {
                myblock += 1;
                nextblockfree = *myblock;
            }
            else {
                nextblockfree = bitmap.get_nextfreeblock(root);
            }
            write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, nextblockfree);
            blockfs.close();
            root.refresh_freeBlocksCount(-1);
            bitmap.set_blockBusy(nextblockfree, root);

            blockfs.open((root.get_root() + "/" + number_to_string(nextblockfree)).c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
            write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT, 0);
            records_count = 0;
        }
        else {
            filerecords_[i].writeAboutSelf(blockfs);
            ++records_count;
        }
    }
    size_t curpos = blockfs.tellg();
    blockfs.seekg(0, blockfs.beg);
    write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT, records_count);
    blockfs.seekg(curpos, blockfs.beg);
    write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, LAST_BLOCK);
    blockfs.close();

    if (myblock != blocks_.end()) {
        for (std::vector<size_t>::iterator it = myblock + 1; it != blocks_.end(); ++it) {
            bitmap.set_blockFree(*it, root);
        }
    }

    for (size_t i = 0; i < filerecords_.size(); ++i) {
        filerecords_[i].writeSelf(root, bitmap);
    }
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        dirrecords_[i].writeSelf(root, bitmap);
    }
}

void Dir::deleteSelf(Root & root, Bitmap & bitmap)
{
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        filerecords_[i].deleteSelf(root, bitmap);
    }
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        dirrecords_[i].deleteSelf(root, bitmap);
    }

    size_t nextblockNumber = startblock_;
    while (nextblockNumber != LAST_BLOCK) {
        std::fstream blockfs ((root.get_root() + "/" + number_to_string(nextblockNumber)).c_str(),
                              std::fstream::in | std::fstream::out | std::fstream::binary);
        blockfs.seekg(get_filerest(blockfs) - BYTES_FOR_BLOCKNUMBER, blockfs.beg);
        size_t bufNumber = read_number_from_bytes(blockfs, BYTES_FOR_BLOCKNUMBER);
        blockfs.close();
        blockfs.open ((root.get_root() + "/" + number_to_string(nextblockNumber)).c_str(),
                      std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
        blockfs.close();
        bitmap.set_blockFree(nextblockNumber, root);
        nextblockNumber = bufNumber;
    }
}

void Dir::add_filerecord(File & fr)
{
    filerecords_.push_back(fr);
}

void Dir::add_dirrecord(Dir & dir)
{
    dirrecords_.push_back(dir);
}

void Dir::copy_filerecord(Root & root, Bitmap & bitmap, File &fr)
{
    size_t freeblockNumber = bitmap.get_nextfreeblock(root);
    bitmap.set_blockBusy(freeblockNumber, root);
    File newfile (fr.name_, 0, get_currenttime(), freeblockNumber);
    newfile.read_fsfile(root, fr);
    filerecords_.push_back(newfile);
}

void Dir::copy_dirrecord(Root & root, Bitmap & bitmap, Dir &dir)
{
    size_t freeblockNumber = bitmap.get_nextfreeblock(root);
    bitmap.set_blockBusy(freeblockNumber, root);
    Dir newdir (dir.name_, 0, get_currenttime(), freeblockNumber);

    for (size_t i = 0; i < dir.filerecords_.size(); ++i) {
        newdir.copy_filerecord(root, bitmap, dir.get_file(i));
    }
    for (size_t i = 0; i < dir.dirrecords_.size(); ++i) {
        newdir.copy_dirrecord(root, bitmap, dir.get_subdir(i));
    }
    dirrecords_.push_back(newdir);
}

size_t Dir::add_filerecord_by_name(std::string const & filename, Root & root, Bitmap & bitmap)
{
    size_t freeblockNumber = bitmap.get_nextfreeblock(root);
    bitmap.set_blockBusy(freeblockNumber, root);
    File newfile (filename, 0, get_currenttime(), freeblockNumber);
    filerecords_.push_back(newfile);

    return filerecords_.size() - 1;
}

size_t Dir::add_dirrecord_by_name(std::string const & dirname, Root & root, Bitmap & bitmap)
{
    size_t freeblockNumber = bitmap.get_nextfreeblock(root);
    bitmap.set_blockBusy(freeblockNumber, root);
    Dir newdir(dirname, 0, get_currenttime(), freeblockNumber);
    dirrecords_.push_back(newdir);

    return dirrecords_.size() - 1;
}

void Dir::remove_file(Root & root, Bitmap & bitmap, size_t filenumber)
{
    get_file(filenumber).deleteSelf(root, bitmap);
    filerecords_.erase(filerecords_.begin() + filenumber, filerecords_.begin() + filenumber + 1);
}

void Dir::remove_subdir(Root & root, Bitmap & bitmap, size_t dirnumber)
{
    get_subdir(dirnumber).deleteSelf(root, bitmap);
    dirrecords_.erase(dirrecords_.begin() + dirnumber, dirrecords_.begin() + dirnumber + 1);
}

size_t Dir::find_file_by_name(std::string const & filename)
{
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        if (filerecords_[i].name_ == filename) {
            return i;
        }
    }
    return LAST_BLOCK;
}

size_t Dir::find_dir_by_name(std::string const & dirname)
{
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        if (dirrecords_[i].name_ == dirname) {
            return i;
        }
    }
    return LAST_BLOCK;
}

Dir & Dir::get_subdir(size_t dirnumber)
{
    return dirrecords_[dirnumber];
}

File & Dir::get_file(size_t filenumber)
{
    return filerecords_[filenumber];
}

void Dir::create_subdir_by_name (Root & root, Bitmap & bitmap, std::string const & subdirName)
{
    size_t nextfreeblock = bitmap.get_nextfreeblock(root);
    bitmap.set_blockBusy(nextfreeblock, root);
    Dir dir (subdirName, 0, get_currenttime(), nextfreeblock);
    dirrecords_.push_back(dir);
}

Dir & Dir::get_lastdir ()
{
    return dirrecords_.back();
}

size_t Dir::get_size()
{
    size_t sum = 0;
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        sum += filerecords_[i].size_;
    }
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        sum += dirrecords_[i].get_size();
    }
    return sum;
}

void Dir::printInfo()
{
    std::cout << "Type: Dir" << std::endl <<
                 "Name: " << name_ << std::endl <<
                 "Size: " << get_size() << " B" << std::endl <<
                 "Date: " << timeinfo_to_string(time_) << std::endl;
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        std::cout << "   --> Dir: " << dirrecords_[i].name_ << std::endl;
    }
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        std::cout << "   --> File: " << filerecords_[i].name_ <<std::endl;
    }
}

























