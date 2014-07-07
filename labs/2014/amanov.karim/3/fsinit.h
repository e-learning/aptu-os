#ifndef FSINIT_H
#define FSINIT_H
#include <istream>
#include <string>
#include "blockwriter.h"

class FsInit
{
public:
  FsInit(std::string root);
  bool parseConfigure(std::istream& input);
  void init();

private:
  void createBlocks();
  void createSuperblock();
  void createDataBitmap();
  void createInodeBitmap();
  void createInodesTable();

  std::string m_initDir;
  size_t m_blockSize;
  size_t m_blocksNum;
  size_t m_dataBitmapBlockNum;
  size_t m_inodesPerBlock;
  size_t m_inodeTableBlockNum;
  size_t m_inodesNum;
  size_t m_inodeBitmapBlockNum;
  size_t m_superblockStart;
  size_t m_dataBitmapStart;
  size_t m_inodeBitmapStart;
  size_t m_inodeTableStart;
  BlockWriter m_writer;
};

#endif // FSINIT_H
