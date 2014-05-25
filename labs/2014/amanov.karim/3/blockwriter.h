#ifndef BLOCKWRITER_H
#define BLOCKWRITER_H
#include <string>
#include <fstream>
#include "bitmap.h"

class BlockWriter
{
public:
  BlockWriter(const std::string& dir);
  void setBlockSize(size_t blockSize);
  void writeBitVector(std::vector<bool> &bits_vector, size_t startBlock, size_t blocksNum);
  void write(const char* data, size_t startBlock, size_t blockNum, size_t nPerBlock, size_t size = 0);

  void writeToCurrentBlock(const char* data, size_t size);
  void openBlock(size_t block);
  void closeCurrentBlock();
  void writeToBlockRun(std::ifstream &from, size_t startBlock, size_t len, size_t &size);
private:
  void write(std::vector<bool>& bits_vector, size_t start, size_t end);

  std::string m_initDir;
  size_t m_blockSize;
  std::ofstream m_writer;

};

#endif // BLOCKWRITER_H
