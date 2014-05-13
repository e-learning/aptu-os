#include "structures.h"
#include "converter.h"
#include <fstream>

Bitmap::Bitmap():
    bitmapCount_(0),
    freeBlocksCount_(0),
    blocksCount_(0),
    bitmapBuf_(NULL)
{}

Bitmap::Bitmap (Root * root)
{
    bitmapCount_ = root->get_bitmapCount();
    freeBlocksCount_ = root->get_freeBlocksCount();
    blocksCount_ = root->get_blocksCount();
    bitmapBuf_ = new char[blocksCount_];
	readSelf(root);
}

Bitmap::Bitmap(Bitmap const & bitmap):
    bitmapCount_(bitmap.bitmapCount_),
    freeBlocksCount_(bitmap.freeBlocksCount_),
    blocksCount_(bitmap.blocksCount_),
    bitmapBuf_(NULL)
{
    if (bitmap.bitmapBuf_ != NULL) {
        bitmapBuf_ = new char[blocksCount_];
        std::copy (bitmap.bitmapBuf_, bitmap.bitmapBuf_ + bitmap.blocksCount_, bitmapBuf_);
    }
}

Bitmap & Bitmap::operator= (Bitmap const & bitmap)
{
    bitmapCount_ = bitmap.bitmapCount_;
    freeBlocksCount_ = bitmap.freeBlocksCount_;
    blocksCount_ = bitmap.blocksCount_;
    bitmapBuf_ = NULL;
    if (bitmap.bitmapBuf_ != NULL) {
        bitmapBuf_ = new char[blocksCount_];
        std::copy (bitmap.bitmapBuf_, bitmap.bitmapBuf_ + bitmap.blocksCount_, bitmapBuf_);
    }

    return *this;
}

Bitmap::~Bitmap()
{ delete[] bitmapBuf_; }

//only in format
void Bitmap::initSelf(Root *root)
{
    bitmapCount_ = root->get_bitmapCount();
    freeBlocksCount_ = root->get_freeBlocksCount();
    blocksCount_ = root->get_blocksCount();
    bitmapBuf_ = new char[blocksCount_];
    std::fill(bitmapBuf_, bitmapBuf_ + blocksCount_, 1);
}

void Bitmap::readSelf(Root * root) //used in constructor: Bitmap(Root * root)
{
    size_t readSize = 0;
    for (size_t i = 1; i <= bitmapCount_; ++i) {
        std::string bitmapBlockName = root->get_rootpath() + "/" + number_to_string(i);
        size_t blockSize = get_filesize_by_name(bitmapBlockName);
        std::fstream bitmapfs (bitmapBlockName.c_str(),
                               std::fstream::in | std::fstream::out | std::fstream::binary);
        bitmapfs.read(bitmapBuf_ + readSize, blockSize);
        readSize += blockSize;
        bitmapfs.close();
    }
}

void Bitmap::writeSelf(Root *root)
{
    size_t writeSize = 0;
    size_t blockSize = root->get_blockSize();
    for (size_t i = 1; i <= bitmapCount_; ++i) {
        size_t writeBytesCount = 0;
        std::string bitmapBlockName = root->get_rootpath() + "/" + number_to_string(i);
        std::fstream bitmapfs (bitmapBlockName.c_str(),
                               std::fstream::in | std::fstream::out
                               | std::fstream::binary | std::fstream::trunc);
        if (blocksCount_ - writeSize < blockSize) {
            writeBytesCount = blocksCount_ - writeSize;
        }
        else {
            writeBytesCount = blockSize;
        }
        bitmapfs.write(bitmapBuf_ + writeSize, writeBytesCount);
        writeSize += writeBytesCount;
        bitmapfs.close();
    }
    root->refresh_freeBlocksCount(freeBlocksCount_);
}

bool Bitmap::hasFreeBlocksCount(size_t requiredBlocksCount)
{
    return freeBlocksCount_ >= requiredBlocksCount;
}

void Bitmap::set_blockFree(size_t blockNumber)
{
    if (blockNumber >= blocksCount_) {
        return;
    }
    char oldState = bitmapBuf_[blockNumber];
    char newState = 1;

    if (oldState != newState) {
        bitmapBuf_[blockNumber] = newState;
        freeBlocksCount_ += 1;
    }
}

void Bitmap::set_blockBusy(size_t blockNumber)
{
    if (blockNumber >= blocksCount_) {
        return;
    }
    char oldState = bitmapBuf_[blockNumber];
    char newState = 0;
    if (oldState != newState) {
        bitmapBuf_[blockNumber] = newState;
        freeBlocksCount_ -= 1;
    }
}

size_t Bitmap::get_freeBlocksCount()
{
    return freeBlocksCount_;
}

size_t Bitmap::get_nextFreeBlock()
{
    if (freeBlocksCount_ == 0) {
		std::cout << "NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl << "NOBLOCKS" << std::endl << "NOBLOCKS" << std::endl <<
			"NOBLOCKS" << std::endl << std::endl;
        return LAST_BLOCK;
    }
    for (size_t i = 0; i < blocksCount_; ++i) {
        if (bitmapBuf_[i] == 1) {
            return i;
        }
    }
    return LAST_BLOCK;
}


