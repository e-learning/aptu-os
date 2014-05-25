#include "blockreader.h"
#include <assert.h>

BlockReader::BlockReader(std::string& dir)
  : m_initDir(dir)
{
  m_reader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
}

void BlockReader::setBlockSize(size_t blockSize)
{
  m_blockSize = blockSize;
}

void BlockReader::readBitVector(std::vector<bool> &v, size_t startBlock, size_t blockNum, size_t size)
{
  size_t cnt = 0;
  for (size_t i = startBlock; i < startBlock + blockNum; i++) {
    openBlock (i);
    size_t min = std::min(8 * (cnt + m_blockSize), size);
    read(v, 8 * cnt, min);
    cnt += m_blockSize;
    closeCurrentBlock ();
    }
}

void BlockReader::read(char *dst, size_t startBlock, size_t blockNum, size_t nPerBlock, size_t size)
{
  if (size == 0) {
      size = blockNum * nPerBlock;
  }
  int cnt = 0;
  for (size_t i = startBlock; i < startBlock + blockNum; i++) {
    openBlock (i);
    size_t min = std::min(nPerBlock, size - cnt * nPerBlock);
    m_reader.read (dst + cnt * nPerBlock, min);

    cnt++;
    closeCurrentBlock ();
    }
}

void BlockReader::read(char *dst, std::vector<BlockRun> &blocks, size_t nPerBlock, size_t size)
{
  for(size_t i = 0; i < blocks.size (); i++) {
      read(dst, blocks[i].blockId, blocks[i].len, nPerBlock, size);
      assert(size > 0);
      size_t readSize = std::min(blocks[i].len * nPerBlock, size);
      dst += readSize;
      size -= readSize;
  }
  assert(size == 0);

}

void BlockReader::readFromCurrentBlock(char *dst, size_t size)
{
  m_reader.read (dst, size);
}

void BlockReader::setPosInCurrentBlock(size_t pos)
{
  m_reader.seekg (pos);
}


void BlockReader::getBlockRunList(const Inode &inode, std::vector<BlockRun> &blockRunsList)
{
  if (!inode.isSingleBlockRun()) {
      BlockRun blockRunArr[inode.multBlockRunNum];
      size_t nPerBlock = (m_blockSize / sizeof(BlockRun)) * sizeof(BlockRun);
      read ((char *) (blockRunArr), inode.data.blockId, inode.data.len,
                     nPerBlock, inode.multBlockRunNum * sizeof(BlockRun));
      for (size_t i = 0; i < inode.multBlockRunNum; i++) {
          blockRunsList.push_back (blockRunArr[i]);
      }
  } else {
    blockRunsList.push_back (inode.data);
  }
}

void BlockReader::readFromBlockRun(std::ofstream &to, size_t startBlock, size_t len, size_t &size)
{
  char buffer[m_blockSize];
  for(size_t block = startBlock; block < startBlock + len; block++) {
      size_t min =  std::min(m_blockSize, size);
      read (buffer, block, 1, min);
      to.write (buffer, min);
      size -= std::min(m_blockSize, size);
    }
}

void BlockReader::read(std::vector<bool> &bits_vector, size_t start, size_t end)
{
  unsigned char buffer = m_reader.get ();
  size_t bitCount = 0;
  for (size_t i = start; i < end; i++) {
      if (bitCount == 8) {
        buffer = m_reader.get ();
        bitCount = 0;
      }
      bits_vector[i] = ((buffer >> bitCount) & 1);
      bitCount++;
    }
}

void BlockReader::openBlock(size_t block)
{
   m_reader.open (m_initDir + "/" + std::to_string (block), std::ios_base::in | std::ios_base::binary);
}

void BlockReader::closeCurrentBlock()
{
  m_reader.close ();
}
