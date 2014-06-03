#include "fsinit.h"
#include "bitmap.h"
#include "inode.h"
#include "superblock.h"
#include <iostream>
#include <fstream>
#include <string>
#include "inode.h"


FsInit::FsInit(std::string root)
  : m_initDir(root)
  , m_writer(root)
{

}

bool FsInit::parseConfigure(std::istream &input)
{
  try {
    input >> m_blockSize >> m_blocksNum;
  }  catch (std::ios_base::failure e) {
    std::cerr << "Error : parse configure" << std::endl;
    return false;
  }
  return true;
}

void FsInit::init()
{
  m_dataBitmapBlockNum = (m_blocksNum/(m_blockSize * 8)) + ((m_blocksNum % (m_blockSize * 8)) ? 1 : 0);
  m_inodesPerBlock = m_blockSize / sizeof(Inode);
  m_inodeTableBlockNum = m_blocksNum / m_inodesPerBlock;
  m_inodesNum = m_inodeTableBlockNum * m_inodesPerBlock;
  m_inodeBitmapBlockNum = (m_inodesNum/(m_blockSize * 8)) + ((m_inodesNum % (m_blockSize * 8)) ? 1 : 0);

  m_superblockStart = 0;
  m_dataBitmapStart = 1;
  m_inodeBitmapStart = m_dataBitmapStart + m_dataBitmapBlockNum;
  m_inodeTableStart = m_inodeBitmapStart + m_inodeBitmapBlockNum;

  m_writer.setBlockSize (m_blockSize);
  createBlocks();
  createSuperblock ();
  createDataBitmap ();
  createInodeBitmap();
  createInodesTable ();
}

void FsInit::createBlocks()
{
  std::ofstream stream;
  for (unsigned int i = 0; i < m_blocksNum; i++) {
        stream.open (m_initDir + "/" + std::to_string (i), std::ios_base::out);
        stream.close ();
  }
}

void FsInit::createSuperblock()
{
  Superblock superblock;
  superblock.blockSize = m_blockSize;
  superblock.blocksNum = m_blocksNum;
  superblock.usedBlocks = 1 + m_dataBitmapBlockNum + m_inodeBitmapBlockNum + m_inodeTableBlockNum;
  superblock.root = 0;
  superblock.dataBitmapBlockNum = m_dataBitmapBlockNum;
  superblock.inodesPerBlock = m_inodesPerBlock;
  superblock.inodeTableBlockNum = m_inodeTableBlockNum;
  superblock.inodesNum = m_inodesNum;
  superblock.inodeBitmapBlockNum = m_inodeBitmapBlockNum;
  superblock.inodeBitmapStart = m_inodeBitmapStart;
  superblock.inodeTableStart = m_inodeTableStart;
  superblock.metaSize = superblock.usedBlocks;

  m_writer.write ((const char*) (&superblock), 0, 1, sizeof(Superblock));
}


void FsInit::createDataBitmap()
{
  Bitmap bitmap(m_blocksNum);
  size_t metaBlocksNum = 1 + m_dataBitmapBlockNum + m_inodeBitmapBlockNum + m_inodeTableBlockNum;
  for (size_t i = 0; i < metaBlocksNum; i++) {
    bitmap.setBusy (i);
  }
  m_writer.writeBitVector (bitmap.bitmap, m_dataBitmapStart, m_dataBitmapBlockNum);
}

void FsInit::createInodeBitmap()
{
  Bitmap bitmap(m_inodesNum);
  bitmap.setBusy (0);
  m_writer.writeBitVector (bitmap.bitmap, m_inodeBitmapStart, m_inodeBitmapBlockNum);
}

void FsInit::createInodesTable()
{
  Inode group[m_inodesNum];
  for (size_t i = 0; i < m_inodesNum; i++) {
      group[i].inodeId = i;
  }

  m_writer.write((const char*)(&group[0]), m_inodeTableStart, m_inodeTableBlockNum, m_inodesPerBlock * sizeof(Inode));
}
