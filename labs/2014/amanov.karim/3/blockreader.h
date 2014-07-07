#ifndef BLOCKREADER_H
#define BLOCKREADER_H
#include <vector>
#include <fstream>
#include <list>
#include "inode.h"

class BlockReader
{
public:
  BlockReader(std::string& dir);
  void setBlockSize(size_t blockSize);
  void readBitVector(std::vector<bool>& v, size_t startBlock, size_t blockNum, size_t size);
  void read(char * dst, size_t startBlock,  size_t blockNum, size_t nPerBlock, size_t size = 0);
  void read(char * dst, std::vector<BlockRun>& blocks, size_t nPerBlock, size_t size);
  void openBlock(size_t block);
  void closeCurrentBlock();
  void readFromCurrentBlock(char* dst, size_t size);
  void setPosInCurrentBlock(size_t pos);
  void getBlockRunList(const Inode& inode,std::vector<BlockRun>& blockRunsList);
  void readFromBlockRun(std::ofstream &to, size_t startBlock, size_t len, size_t &size);
private:
  void read(std::vector<bool>& bits_vector, size_t start, size_t end);

  std::string m_initDir;
  size_t m_blockSize;
  std::ifstream m_reader;

};

#endif // BLOCKREADER_H
