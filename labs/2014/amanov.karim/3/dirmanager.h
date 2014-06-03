#ifndef DIRMANAGER_H
#define DIRMANAGER_H
#include <string>
#include "superblock.h"
#include "bitmap.h"
#include "inode.h"
#include "blockwriter.h"
#include "blockreader.h"
#include <cstdlib>
#include <deque>

struct DirEntry {
  DirEntry() {
    for (size_t i = 0; i < 10; i++) {
        name[i] = '\0';
      }
  }

  bool isEqualName(const std::string& str) {
    size_t i = 0;
    while (i < 11 && name[i] != '\0') {
      if (name[i] != str[i]) {
        return false;
      }
      i++;
    }
    if (i == str.size ()) {
      return true;
    } else {
        return false;
    }
  }

  char name[11];
  size_t inodeId;
  void setName(std::string fName) {
    fName.copy (&name[0], fName.size ());
  }
  void setInodeId(size_t id) {
    inodeId = id;
  }
};

class FsController;
class DirManager
{
public:
  DirManager(Superblock& superblock, Bitmap& dataBitmap,
             Bitmap& inodeBitmap, BlockWriter& w, BlockReader& r);
  void setController(FsController* c);
  void writeNewDirInfo(size_t block, size_t id, size_t parent_id);
  static bool parsePath(std::string& path, std::deque<std::string>& out);
  static const char SEPARATOR;
  size_t getDirFileInodeId(const Inode& dirInode, const std::string& fileName, bool isDir = false);
  void getDirFiles(const Inode &dirInode, std::vector<DirEntry> &dirFiles);
  bool addFileToDir(Inode& dirInode, Inode& fileInode, const std::string& fileName, bool isUpdate = true);
  std::string getFileName(const Inode& fileInode);
  bool removeFileFromDir(Inode &dirInode, const Inode& inode);
private:
  FsController* m_controller;
  Superblock& m_superblock;
  Bitmap& m_dataBitmap;
  Bitmap& m_inodeBitmap;
  BlockWriter& m_writer;
  BlockReader& m_reader;
};

#endif // DIRMANAGER_H
