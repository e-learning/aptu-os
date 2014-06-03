#include "blocklistiterator.h"


BlockListIterator::BlockListIterator(std::vector<BlockRun> &runsList)
  : blockRunsList(runsList)
  , blocksNum(0)
  , currentPos(0)
{
  for (size_t i = 0; i < blockRunsList.size (); i++) {
      blocksNum += blockRunsList[i].len;
  }
}

bool BlockListIterator::hasNext()
{
  return currentPos < blocksNum;
}

size_t BlockListIterator::getNext()
{
  long long cnt = currentPos + 1;
  size_t offset = 0;
  size_t blockRunInd = 0;
  for (size_t i = 0; i < blockRunsList.size (); i++) {
      cnt -= blockRunsList[i].len;    
      if (cnt <= 0) {
          blockRunInd = i;
          offset = currentPos - offset;
          break;
      }
      offset += blockRunsList[i].len;
  }
  currentPos++;
  return blockRunsList[blockRunInd].blockId + offset;
}
