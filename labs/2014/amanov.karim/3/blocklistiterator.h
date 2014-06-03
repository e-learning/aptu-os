#ifndef BLOCKLISTITERATOR_H
#define BLOCKLISTITERATOR_H
#include <vector>
#include "inode.h"

class BlockListIterator
{
public:
  BlockListIterator(std::vector<BlockRun>& runsList);
  bool hasNext();
  size_t getNext();
private:
  std::vector<BlockRun>& blockRunsList;
  size_t blocksNum;
  size_t currentPos;
};

#endif // BLOCKLISTITERATOR_H
