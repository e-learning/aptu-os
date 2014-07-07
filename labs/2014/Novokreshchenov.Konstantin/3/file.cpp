#include "structures.h"
#include "converter.h"
#include <iostream>
#include <iomanip>

File::File():
    name_(""),
    size_(0),
    time_(0),
    startblock_(LAST_BLOCK),
    buf_(NULL)
{}

File::File(std::string const & name, size_t size, size_t time, size_t startblock) :
    name_(name),
    size_(size),
    time_(time),
    startblock_(startblock),
    buf_(NULL)
{
    blocks_.push_back(startblock_);
}

File::File (File const & file):
    name_(file.name_),
    size_(file.size_),
    time_(file.time_),
    startblock_(file.startblock_),
    blocks_(file.blocks_),
    buf_(NULL)
{
    if (file.buf_ != NULL) {
        buf_ = new char[size_];
        std::copy(file.buf_, file.buf_ + file.size_, buf_);
    }
}

File & File::operator= (File const & file)
{
    name_ = file.name_;
    size_ = file.size_;
    time_ = file.time_;
    startblock_ = file.startblock_;
    blocks_ = file.blocks_;
    buf_ = NULL;
    if (file.buf_ != NULL) {
        buf_ = new char[size_];
        std::copy(file.buf_, file.buf_ + file.size_, buf_);
    }

    return *this;
}

File::~File()
{ delete[] buf_; }

size_t File::get_sizeInBlocks() {
    return blocks_.size();
}

size_t File::get_size()
{
    return size_;
}

void File::readAboutSelf(std::fstream & fs)
{
    name_ = read_string_from_bytes(fs, BYTES_FOR_NAME);
    size_ = read_number_from_bytes(fs, BYTES_FOR_FILESIZE);
    time_ = read_number_from_bytes(fs, BYTES_FOR_TIME);
    startblock_ = read_number_from_bytes(fs, BYTES_FOR_BLOCKNUMBER);
    blocks_.clear();
    blocks_.push_back(startblock_);
}

void File::readSelf(Root * root)
{
	//std::cout << "Reading File " << name_ << std::endl;

	if (size_ <= 0) {
		return;
	}

    delete buf_;
    buf_ = new char[size_];
    size_t currentBlockNumber = startblock_;
    size_t readSize = 0;
    while (true) {
		//std::cout << currentBlockNumber << ", ";
        std::string currentBlockName = root->get_rootpath() + "/" + number_to_string(currentBlockNumber);
        size_t blockSize = get_filesize_by_name(currentBlockName);
        std::fstream currentBlockfs(currentBlockName.c_str(),
                                    std::fstream::in | std::fstream::out | std::fstream::binary);
        currentBlockfs.read(buf_ + readSize, blockSize - BYTES_FOR_BLOCKNUMBER);
        readSize += blockSize - BYTES_FOR_BLOCKNUMBER;
        currentBlockNumber = read_number_from_bytes(currentBlockfs, BYTES_FOR_BLOCKNUMBER);
        currentBlockfs.close();
        if (currentBlockNumber == LAST_BLOCK) {
            break;
        }
        blocks_.push_back(currentBlockNumber);
    }
	//std::cout << std::endl << std::endl;
}

void File::read_hostfile(std::string const & hostfile)
{
    size_t filesize = get_filesize_by_name(hostfile);
    delete buf_;
    buf_ = new char[filesize];
    std::fstream hostfs(hostfile.c_str(),
                        std::fstream::in | std::fstream::out | std::fstream::binary);
    hostfs.read(buf_, filesize);
}

void File::read_fsfile(File * file)
{
    size_ = file->size_;
    delete buf_;
    buf_ = NULL;
    if (file->buf_ != NULL) {
        buf_ = new char[file->size_];
        std::copy (file->buf_, file->buf_ + file->size_, buf_);
    }
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
    write_string_to_bytes(fs, BYTES_FOR_NAME, name_);
    write_number_to_bytes(fs, BYTES_FOR_BLOCKSIZE, size_);
    write_number_to_bytes(fs, BYTES_FOR_TIME, time_);
    write_number_to_bytes(fs, BYTES_FOR_BLOCKNUMBER, startblock_);
}

void File::writeSelf(Root * root, Bitmap * bitmap)
{
	//std::cout << "Writing File " << name_ << std::endl;

    BlockIt blockIt = blocks_.begin();
    size_t writeSize = 0;
    size_t blockSize = root->get_blockSize();
    size_t currentBlockNumber = get_nextBlockForWrite(blockIt, bitmap);
	std::string currentBlockName = root->get_rootpath() + "/" + number_to_string(currentBlockNumber);
	std::fstream blockfs(currentBlockName.c_str(),
		std::fstream::out | std::fstream::binary | std::fstream::trunc);
    while (true) { //trunc
		//std::cout << currentBlockNumber << ", ";
		size_t writebytesCount = size_ - writeSize < blockSize - BYTES_FOR_BLOCKNUMBER 
			? size_ - writeSize: blockSize - BYTES_FOR_BLOCKNUMBER;
        blockfs.write(buf_ + writeSize, writebytesCount);
        writeSize += writebytesCount;
		if (writeSize == size_) {
			write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, LAST_BLOCK);
			blockfs.close();
			break;
		}
		else {
			currentBlockNumber = get_nextBlockForWrite(blockIt, bitmap);
			write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, currentBlockNumber);
			blockfs.close();
			currentBlockName = root->get_rootpath() + "/" + number_to_string(currentBlockNumber);
			blockfs.open(currentBlockName.c_str(),
				std::fstream::out | std::fstream::binary | std::fstream::trunc);
		}
    }

    if (blockIt != blocks_.end()) {
        for (std::vector<size_t>::iterator it = blockIt; it != blocks_.end(); ++it) {
            bitmap->set_blockFree(*it);
        }
    }
}

size_t File::get_nextBlockForWrite(BlockIt & it, Bitmap * bitmap)
{
	size_t blockNumber = LAST_BLOCK;
	if (it != blocks_.end()) {
		blockNumber = *it;
		++it;
	}
	else {
		blockNumber = bitmap->get_nextFreeBlock();
	}
	bitmap->set_blockBusy(blockNumber);
	return blockNumber;
}

void File::deleteSelf(Root * root, Bitmap * bitmap)
{
    for (BlockIt it = blocks_.begin(); it != blocks_.end(); ++it) {
        std::string truncedBlockName = root->get_rootpath() + "/" + number_to_string(*it);
        std::fstream blockfs (truncedBlockName.c_str(),
                              std::fstream::out | std::fstream::binary | std::fstream::trunc);
        bitmap->set_blockFree(*it);
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











