#ifndef FSCONTROLLER_H
#define FSCONTROLLER_H
#include "superblock.h"
#include "bitmap.h"
#include "blockwriter.h"
#include "inode.h"
#include "blockreader.h"
#include "dirmanager.h"
#include <queue>
#include <deque>

class FsController
{
public:
  static const size_t MAX_NAME_LENGTH = 10;
  FsController(std::string& initDir);
  ~FsController();
  void mount();
  bool format();
  bool import(std::string& host_file, std::string& fs_file);
  bool exportFile(std::string& fs_file, std::string& host_file);
  bool ls(std::string& fs_file);
  bool copy(std::string& src, std::string& dst);
  bool mkdir(std::string& fs_file);
  void unmount();
  bool move(std::string& src, std::string& dst);
  bool rm(std::string& file);
  void updateSize(Inode fileInode, long long dataSize);
private:
  void freeResources(Inode& srcInode);
  void freeData(Inode& srcInode);
  void copyBlock(size_t from, size_t to, size_t size);
  bool copyData(Inode& srcInode, Inode& copyInode);
  bool doMkDir(Inode& dstInode, Inode& newDir, std::string& fileName);
  bool copyFile(Inode& srcInode, Inode& dstInode, std::string& fileName);
  bool moveFile(Inode& srcInode, Inode& dstInode, std::string& fileName);
  bool doExportFile(Inode& fileInode, std::string& host_file);
  void writeToBlockRun(std::ifstream& from, size_t startBlock, size_t len, size_t& size);
  void readFromBlockRun(std::ofstream& to, size_t startBlock, size_t len, size_t& size);
  bool allocateMemory(std::vector<BlockRun>& allocBlocks, size_t fileLength);
  bool createFreeList(std::list<BlockRun>& freeList, size_t requiredBlocks);
  bool getInode(std::deque<std::string>& path, Inode& inode);
  void readSuperblock();
  bool removeFileFromDir(Inode& srcInode);
  void readDataBitmap();
  void readInodeBitmap();
  bool createRootDir();
  bool getFileName(Inode& fileInode, std::string& name);
  bool importFile(Inode& inode, std::string& filename);
  void readInode(Inode& inode, size_t inodeId);
  void writeInode(Inode& inode);
  size_t getFileLength(std::ifstream& file);
  std::string m_initDir;
  BlockReader m_reader;
  BlockWriter m_writer;
  Superblock m_superblock;
  Bitmap m_dataBitmap;
  Bitmap m_inodeBitmap;
  DirManager* m_dirManager;

};

#endif // FSCONTROLLER_H
