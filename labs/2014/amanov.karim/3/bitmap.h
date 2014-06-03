#ifndef BITMAP_H
#define BITMAP_H
#include <vector>
#include <cstdlib>
#include "inode.h"

struct Bitmap
{
  Bitmap();
  Bitmap(size_t size);
  void init(size_t size);
  void setBusy(size_t index);
  void setBusy(BlockRun& blockRun);
  void setFree(size_t index);
  bool isFree(size_t index);
  bool getAvaible(size_t& val, size_t start = 0);
  std::vector<bool> bitmap;
};

#endif // BITMAP_H
