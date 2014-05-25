#include "bitmap.h"

Bitmap::Bitmap()
{
}

Bitmap::Bitmap(size_t size)
  : bitmap(size, false)
{
}

void Bitmap::init(size_t size)
{
  bitmap.resize (size);
  for(size_t i = 0; i < size; i++) {
      bitmap[i] = false;
  }
}

void Bitmap::setBusy(size_t index)
{
  bitmap[index] = true;
}

void Bitmap::setBusy(BlockRun &blockRun)
{
  for (size_t i = blockRun.blockId; i < blockRun.blockId + blockRun.len; i++) {
    setBusy (i);
  }
}

void Bitmap::setFree(size_t index)
{
  bitmap[index] = false;
}

bool Bitmap::isFree(size_t index)
{
  return !bitmap[index];
}

bool Bitmap::getAvaible(size_t& val, size_t start)
{
  size_t size = bitmap.size ();
  for (size_t i = start; i < size; i++) {
      if (!bitmap[i]) {
          val = i;
          return true;
      }
  }
  return false;

}
