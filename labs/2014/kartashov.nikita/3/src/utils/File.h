#ifndef FILE_H_
#define FILE_H_

#include <ctime>

struct File
{
  int id;
  bool isDirectory;
  size_t sizeInBlocks;
  int prev;
  int next;
  int parent;
  int firstChild;
  int firstBlock;
  char name[10];
  time_t lastModified;
};

#endif /* end of include guard */

