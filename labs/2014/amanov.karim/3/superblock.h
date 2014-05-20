#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include <cstdlib>

struct Superblock
{
  size_t blockSize;
  size_t blocksNum;
  size_t usedBlocks;
  size_t root;
  size_t dataBitmapBlockNum;
  size_t inodesPerBlock;
  size_t inodeTableBlockNum;
  size_t inodesNum;
  size_t inodeBitmapBlockNum;
  size_t inodeBitmapStart;
  size_t inodeTableStart;
  size_t metaSize;
};

#endif // SUPERBLOCK_H
