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

/*==Root==*/
Root::Root (std::string root, size_t freeBlocksCount, size_t blockSize, size_t bitmapCount, size_t blocksCount, Dir rootDir):
    root_(root), freeBlocksCount_(freeBlocksCount), blockSize_(blockSize), bitmapCount_(bitmapCount), blocksCount_(blocksCount), rootDir_(rootDir)
{}

void Root::initSelf()
{
    std::fstream rootfs ((root_ + "/" + number_to_string(ROOT_BLOCK_NUMBER)).c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary);
    freeBlocksCount_ = read_number_from_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT);
    blockSize_ = read_number_from_bytes(rootfs, BYTES_FOR_BLOCKSIZE);
    bitmapCount_ = read_number_from_bytes(rootfs, BYTES_FOR_BITMAP_COUNT);
    blocksCount_ = read_number_from_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT);
    rootDir_.readAboutSelf(rootfs);
    rootfs.close();
}

void Root::refresh_freeBlocksCount (int diff)
{
    freeBlocksCount_ += diff;
}

void Root::writeSelf()
{
    std::fstream rootfs ((root_ + "/" + number_to_string(ROOT_BLOCK_NUMBER)).c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT, freeBlocksCount_);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKSIZE, blockSize_);
    write_number_to_bytes(rootfs, BYTES_FOR_BITMAP_COUNT, bitmapCount_);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKS_COUNT, blocksCount_);
    rootDir_.writeAboutSelf(rootfs);
    write_number_to_bytes(rootfs, BYTES_FOR_BLOCKNUMBER, LAST_BLOCK);
    rootfs.close();
}

/*==Bitmap==*/
void Bitmap::initWriteSelf(Root & root)
{
    size_t lastbitmapblocksize = (root.get_blocksCount() % (root.get_blockSize() * BITS_PER_BYTE)) / BITS_PER_BYTE + 1;
    for (size_t i = 1; i < bitmapCount_; ++i) {
        std::fstream bitmapblockfs ((root.get_root() + "/" + number_to_string(i)).c_str(),
                                    std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
        fill_block_by_char(bitmapblockfs, root.get_blockSize(), (char)MAX_BYTE);
        bitmapblockfs.close();
    }
    std::fstream bitmapblockfs ((root.get_root() + "/" + number_to_string(bitmapCount_)).c_str(),
                                std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    fill_block_by_char(bitmapblockfs, lastbitmapblocksize, (char)MAX_BYTE);
    bitmapblockfs.close();

    for (size_t i = 0; i < root.get_bitmapCount() + 1; ++i) {
        set_blockBusy(i, root);
    }
}

size_t Bitmap::get_nextfreeblock(Root & root)
{
    if (root.get_freeBlockscount() == 0) {
        return LAST_BLOCK;
    }
    for (size_t i = 0; i < bitmapCount_; ++i) {
        std::fstream bitmapblockfs ((root.get_root() + "/" + number_to_string(i + 1)).c_str(),
                                    std::fstream::in | std::fstream::out | std::fstream::binary);
        size_t bitmapblockSize = get_filerest(bitmapblockfs);
        char byte;
        for (size_t j = 0; j < bitmapblockSize; ++j) {
            bitmapblockfs.read(&byte, 1);
            size_t pos = find_one_in_byte(byte);
            if (pos != BITS_PER_BYTE) {
                return BITS_PER_BYTE * j + pos;
            }
        }
        bitmapblockfs.close();
    }
    return LAST_BLOCK;
}

void Bitmap::set_blockBusy(size_t blockNumber, Root & root)
{
    size_t bitmapBlockNumber = blockNumber / root.get_blockSize() + 1;
    size_t blockNumberInBitmapblock = blockNumber % root.get_blockSize();
    size_t offset = blockNumberInBitmapblock / BITS_PER_BYTE;
    size_t left_bit_busy = blockNumberInBitmapblock % BITS_PER_BYTE;

    std::fstream bitmapblockfs ((root.get_root() + "/" + number_to_string(bitmapBlockNumber)).c_str(),
                                std::fstream::in | std::fstream::out | std::fstream::binary);
    bitmapblockfs.seekg(offset, bitmapblockfs.beg);
    char byte;
    bitmapblockfs.get(byte);
    char newbyte = set_bit_busy(byte, left_bit_busy);
    if (newbyte != byte) {
        bitmapblockfs.seekg(-1, bitmapblockfs.cur);
        bitmapblockfs.put(newbyte);
        root.refresh_freeBlocksCount(-1);
    }
    bitmapblockfs.close();
}

void Bitmap::set_blockFree(size_t blockNumber, Root & root)
{
    size_t bitmapBlockNumber = blockNumber / root.get_blockSize() + 1;
    size_t blockNumberInBitmapblock = blockNumber % root.get_blockSize();
    size_t offset = blockNumberInBitmapblock / BITS_PER_BYTE;
    size_t left_bit_free = blockNumberInBitmapblock % BITS_PER_BYTE;

    std::fstream bitmapblockfs ((root.get_root() + "/" + number_to_string(bitmapBlockNumber)).c_str(),
                                std::fstream::in | std::fstream::out | std::fstream::binary);
    bitmapblockfs.seekg(offset, bitmapblockfs.beg);
    char byte;
    bitmapblockfs.get(byte);
    char newbyte = set_bit_free(byte, left_bit_free);
    if (newbyte != byte) {
        bitmapblockfs.seekg(-1, bitmapblockfs.cur);
        bitmapblockfs.put(newbyte);
        root.refresh_freeBlocksCount(+1);
    }
    bitmapblockfs.close();
}

