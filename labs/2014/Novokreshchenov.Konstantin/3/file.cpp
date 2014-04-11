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


/*==File==*/
void File::readAboutSelf(std::fstream & fs)
{
    read_number_from_bytes(fs, BYTES_FOR_ISDIR);
    name_ = read_string_from_bytes(fs, BYTES_FOR_NAME);
    size_ = read_number_from_bytes(fs, BYTES_FOR_FILESIZE);
    time_ = read_number_from_bytes(fs, BYTES_FOR_TIME);
    startblock_ = read_number_from_bytes(fs, BYTES_FOR_BLOCKNUMBER);
    blocks_.clear();
    blocks_.push_back(startblock_);
}

void File::readSelf(Root & root)
{
    delete buf_;
    //blocks_.push_back(startblock_);
    std::fstream blockfs ((root.get_root() + "/" + number_to_string(startblock_)).c_str(),
                          std::fstream::in | std::fstream::out | std::fstream::binary);
    buf_ = new char[size_];
    size_t readSize = 0;
    while (true) {
        if (size_ - readSize > root.get_blockSize() - BYTES_FOR_BLOCKNUMBER) {
            blockfs.read(buf_ + readSize, root.get_blockSize() - BYTES_FOR_BLOCKNUMBER);
            readSize += root.get_blockSize() - BYTES_FOR_BLOCKNUMBER;
            size_t nextblocknumber = read_number_from_bytes(blockfs, BYTES_FOR_BLOCKNUMBER);
            if (nextblocknumber == LAST_BLOCK) {
                break;
            }
            blockfs.close();
            blockfs.open((root.get_root() + "/" + number_to_string(nextblocknumber)).c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary);
            blocks_.push_back(nextblocknumber);
        }
        else {
            blockfs.read(buf_ + readSize, size_ - readSize);
            readSize += size_ - readSize;
            break;
        }
    }
    blockfs.close();
}

size_t File::read_hostfile(std::string const & hostfile)
{
    size_t filesize = get_filesize_by_name(hostfile);
    std::fstream hostfs(hostfile.c_str(),
        std::fstream::in | std::fstream::out | std::fstream::binary);
    if (hostfs.fail()) {
        return ERROR_INCORRECT_PATH;
    }
    delete buf_;
    buf_ = new char[filesize];
    hostfs.read(buf_, filesize);
    hostfs.close();

    return SUCCESS;
}

void File::read_fsfile(Root & root, File & file)
{
    file.readSelf(root);
    delete buf_;
    buf_ = new char[file.size_];
    size_ = file.size_;
    std::copy (file.buf_, file.buf_ + file.size_, buf_);
}

void File::write_hostfile (std::string const & hostfile)
{
    std::fstream hostfs (hostfile.c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    hostfs.write(buf_, size_);
    hostfs.close();
}

void File::writeAboutSelf(std::fstream & fs)
{
    write_number_to_bytes(fs, BYTES_FOR_ISDIR, 0);
    write_string_to_bytes (fs, BYTES_FOR_NAME, name_);
    write_number_to_bytes(fs, BYTES_FOR_FILESIZE, size_);
    write_number_to_bytes(fs, BYTES_FOR_TIME, time_);
    write_number_to_bytes(fs, BYTES_FOR_BLOCKNUMBER, startblock_);
}

//block using
void File::writeSelf (Root & root, Bitmap & bitmap)
{
    std::fstream blockfs ((root.get_root() + "/" + number_to_string(startblock_)).c_str(),
                          std::fstream::in | std::fstream::out | std::fstream::binary);
    bitmap.set_blockBusy(startblock_, root);
    std::vector<size_t>::iterator myblockIt = blocks_.begin();
    size_t nextfreeblocknumber = *myblockIt;
    size_t writeSize = 0;
    while (true) {
        if (size_ - writeSize > root.get_blockSize() - BYTES_FOR_BLOCKNUMBER) {
            blockfs.write(buf_ + writeSize, root.get_blockSize() - BYTES_FOR_BLOCKNUMBER);
            writeSize += root.get_blockSize() - BYTES_FOR_BLOCKNUMBER;
            if (writeSize < size_) {
                if (myblockIt == blocks_.end() - 1) {
                    nextfreeblocknumber = bitmap.get_nextfreeblock(root);
                }
                else {
                    myblockIt += 1;
                    nextfreeblocknumber = *myblockIt;
                }
                write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, nextfreeblocknumber);
                blockfs.close();
                bitmap.set_blockBusy(nextfreeblocknumber, root);
                blockfs.open((root.get_root() + "/" + number_to_string(nextfreeblocknumber)).c_str(),
                             std::fstream::in | std::fstream::out | std::fstream::binary);
            }
            else {
                write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, LAST_BLOCK);
                break;
            }
        }
        else {
            blockfs.write(buf_ + writeSize, size_ - writeSize);
            writeSize += size_ - writeSize;
            write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, LAST_BLOCK);
            break;
        }
    }
    blockfs.close();
}

void File::deleteSelf(Root & root, Bitmap & bitmap)
{
    size_t nextblockNumber = startblock_;
    while (nextblockNumber != LAST_BLOCK) {
        bitmap.set_blockFree(nextblockNumber, root);
        std::fstream blockfs ((root.get_root() + "/" + number_to_string(nextblockNumber)).c_str(),
                              std::fstream::in | std::fstream::out | std::fstream::binary);
        blockfs.seekg((int)BYTES_FOR_BLOCKNUMBER * (-1), blockfs.end);
        size_t oldBlockNumer = nextblockNumber;
        nextblockNumber = read_number_from_bytes(blockfs, BYTES_FOR_BLOCKNUMBER);
        blockfs.close();
        blockfs.open((root.get_root() + "/" + number_to_string(oldBlockNumer)).c_str(),
                                      std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
        blockfs.close();
    }
}

void File::printInfo()
{
    std::cout << "File: " <<
                 std::setw(10) << name_ <<
                 std::setw(15) << size_ << " B" <<
                 std::setw(35) << timeinfo_to_string(time_) <<
                 std::endl;
}






















