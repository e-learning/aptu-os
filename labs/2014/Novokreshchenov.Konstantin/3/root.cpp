#include "structures.h"
#include "converter.h"

//only in format
Root::Root (std::string root,
            size_t freeBlocksCount,
            size_t blockSize,
            size_t bitmapCount,
            size_t blocksCount,
            Dir * rootDir):
    rootpath_(root),
    freeBlocksCount_(freeBlocksCount),
    blockSize_(blockSize),
    bitmapCount_(bitmapCount),
    blocksCount_(blocksCount),
    rootDir_(rootDir)
{}

Root::Root (std::string const & rootpath): rootpath_(rootpath)
{
    initSelf();
}

void Root::initSelf()
{
    std::fstream rootfs ((rootpath_ + "/" + number_to_string(ROOT_BLOCK_NUMBER)).c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary);
    freeBlocksCount_ = read_number_from_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT);
    blockSize_ = read_number_from_bytes(rootfs, BYTES_FOR_BLOCKSIZE);
    bitmapCount_ = read_number_from_bytes(rootfs, BYTES_FOR_BITMAP_COUNT);
    blocksCount_ = read_number_from_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT);
    read_number_from_bytes(rootfs, BYTES_FOR_ISDIR);
    rootDir_ = new Dir();
    rootDir_->readAboutSelf(rootfs);
    rootfs.close();
}

Root::~Root()
{
    delete rootDir_;
}

std::string const & Root::get_rootpath() const
{ return rootpath_; }

size_t Root::get_freeBlocksCount() const
{ return freeBlocksCount_; }

size_t Root::get_blockSize() const
{ return blockSize_; }

size_t Root::get_bitmapCount() const
{ return bitmapCount_; }

size_t Root::get_blocksCount() const
{ return blocksCount_; }

size_t Root::get_freeSpace() const
{ return freeBlocksCount_ * (blockSize_ - BYTES_FOR_BLOCKNUMBER); }

size_t Root::get_rootRecordStartBlock() const
{ return bitmapCount_ + 1; }

Dir * Root::get_rootDir ()
{ return rootDir_; }

void Root::refresh_freeBlocksCount (size_t freeBlocksCount)
{
    freeBlocksCount_ = freeBlocksCount;
}

void Root::writeSelf()
{
    std::fstream rootfs ((rootpath_ + "/" + number_to_string(ROOT_BLOCK_NUMBER)).c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT, freeBlocksCount_);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKSIZE, blockSize_);
    write_number_to_bytes(rootfs, BYTES_FOR_BITMAP_COUNT, bitmapCount_);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT, blocksCount_);
    rootDir_->writeAboutSelf(rootfs);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKNUMBER, LAST_BLOCK);
    rootfs.close();
}
