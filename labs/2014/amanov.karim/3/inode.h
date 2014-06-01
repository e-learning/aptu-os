#ifndef INODE_H
#define INODE_H
#include <ctime>

struct BlockRun
{
  BlockRun()
    : blockId(0)
    , len(0)
  {
  }

  BlockRun(size_t id, size_t l)
    : blockId(id)
    , len(l)
  {}
  size_t blockId;
  size_t len;
};





struct Inode
{

  Inode()
    : inodeId(0)
    , dataSize(0)
    , lastModifiedTime(0)
    , multBlockRunNum(0)
    , directoryFiles(0)
    , parentId(0)
  {
  }
  size_t inodeId;
  size_t dataSize;
  BlockRun data;
  time_t lastModifiedTime;
  size_t multBlockRunNum;
  size_t directoryFiles;
  size_t parentId;
  bool isSingleBlockRun() const {
    return multBlockRunNum == 0;
  }
  bool isDirectory() const {
    return directoryFiles != 0;
  }

};

#endif // INODE_H
