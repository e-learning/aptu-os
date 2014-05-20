#include "blockwriter.h"


BlockWriter::BlockWriter(const std::string &dir)
  : m_initDir(dir)
{
  m_writer.exceptions(std::ifstream::failbit | std::ifstream::badbit);
}

void BlockWriter::setBlockSize(size_t blockSize)
{
  m_blockSize = blockSize;
}

void BlockWriter::writeBitVector(std::vector<bool> &bits_vector, size_t startBlock, size_t blocksNum)
{
  size_t cnt = 0;
  for (size_t i = startBlock; i < startBlock + blocksNum; i++) {
      openBlock (i);
      write (bits_vector, 8 * cnt, std::min(8 * (cnt + m_blockSize), bits_vector.size ()));
      cnt += m_blockSize;
      closeCurrentBlock ();
  }
}

void BlockWriter::write(const char *data, size_t startBlock, size_t blockNum, size_t nPerBlock, size_t size)
{
  if (size == 0) {
      size = blockNum * nPerBlock;
  }
  int cnt = 0;
  for (size_t i = startBlock; i < startBlock + blockNum; i++) {
      openBlock (i);
      m_writer.write (data + cnt * nPerBlock, std::min(nPerBlock, size - cnt * nPerBlock));
      cnt++;
      closeCurrentBlock ();
    }
}

void BlockWriter::writeToCurrentBlock(const char *data, size_t size)
{
  m_writer.write (data, size);
}

void BlockWriter::write(std::vector<bool>& bits_vector, size_t start, size_t end)
{
    unsigned char buffer = 0;
    size_t bit_count = 0;
    for(size_t i = start; i < end; i++) {
        if(bit_count == 8) {
            m_writer.put (buffer);
            buffer = 0;
            bit_count = 0;
        }
        buffer = buffer | bits_vector[i] << bit_count;
        bit_count++;
    }

    if (bit_count > 0) {
        m_writer.put (buffer);
    }

}

void BlockWriter::writeToBlockRun(std::ifstream& from, size_t startBlock, size_t len, size_t& size) {
  char buffer[m_blockSize];
  for(size_t block = startBlock; block < startBlock + len; block++) {
      size_t min = std::min(m_blockSize, size);
      from.read (buffer, min);
      write (buffer, block, 1, min);
      size -= min;
    }
}

void BlockWriter::openBlock(size_t block)
{
  m_writer.open (m_initDir + "/" + std::to_string (block), std::ios_base::out | std::ios_base::binary);
}

void BlockWriter::closeCurrentBlock()
{
  m_writer.close ();
}
