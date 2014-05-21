#include "fscontroller.h"
#include "dirmanager.h"
#include <iostream>
#include <ctime>
#include <assert.h>
#include "blocklistiterator.h"
FsController::FsController(std::string& initDir)
  : m_initDir(initDir)
  , m_reader(initDir)
  , m_writer(initDir)
  , m_dirManager(0)
{
}

FsController::~FsController()
{
  delete m_dirManager;
}

void FsController::mount()
{
  readSuperblock();
  m_reader.setBlockSize (m_superblock.blockSize);
  m_writer.setBlockSize (m_superblock.blockSize);

  readDataBitmap();
  readInodeBitmap();
  m_dirManager = new DirManager(m_superblock, m_dataBitmap, m_inodeBitmap, m_writer, m_reader);
  m_dirManager->setController (this);
}

bool FsController::format()
{
  if (m_superblock.root != 0) {
      std::cerr << "Error : fs already formated" << std::endl;
      return false;
    }
  if (!createRootDir()) {
      std::cerr << "Error : format fail" << std::endl;
      return false;
    }
  return true;

}

bool FsController::import(std::string& host_file, std::string& fs_file)
{
  std::deque<std::string> path;
  if (fs_file[fs_file.size () - 1] == DirManager::SEPARATOR) {
      std::cerr << "Fs file is a dir" << std::endl;
      return false;
  }

  if (!DirManager::parsePath (fs_file, path)) {
      std::cerr << "fs file path error" << std::endl;
      return false;
  }

  std::string fileName = path.back ();
  path.pop_back ();
  Inode dirInode;
  if (!getInode(path, dirInode)) {
      return false;
  }

  Inode fileInode;
  if (!m_inodeBitmap.getAvaible (fileInode.inodeId)) {
      std::cerr << "Error : no empty inodes" << std::endl;
      return false;
  }

  if (!importFile(fileInode, host_file)) {
      return false;
  }

  if (!m_dirManager->addFileToDir(dirInode, fileInode, fileName)) {
      return false;
    }
  fileInode.lastModifiedTime = std::time(NULL);
  dirInode.lastModifiedTime = std::time(NULL);
  writeInode (fileInode);
  writeInode (dirInode);
  m_inodeBitmap.setBusy (fileInode.inodeId);
  return true;

}

bool FsController::exportFile(std::string& fs_file, std::string& host_file)
{
  std::deque<std::string> path;
  if (fs_file[fs_file.size () - 1] == DirManager::SEPARATOR) {
      std::cerr << "Fs file is a dir" << std::endl;
      return false;
  }

  if (!DirManager::parsePath (fs_file, path)) {
      std::cerr << "fs file path error" << std::endl;
      return false;
  }


  Inode fileInode;
  if (!getInode(path, fileInode)) {
      return false;
  }

  if (fileInode.isDirectory ()) {
      std::cerr << "Error : " << fs_file << " is a directory" << std::endl;
      return false;
  }

  if (!doExportFile(fileInode, host_file)) {
      return false;
  }
  return true;

}

bool FsController::ls(std::string &fs_file)
{
  std::deque<std::string> path;
  std::string fileName;

  if (!DirManager::parsePath (fs_file, path)) {
      std::cerr << "fs file path error" << std::endl;
      return false;
  }

  if (fs_file == "/") {
      fileName = "/";
  } else {
      fileName = path.front ();
  }

  Inode fileInode;
  if (!getInode(path, fileInode)) {
      return false;
  }

  std::cout << fileName << " " << fileInode.dataSize
            << " " << asctime(localtime(&fileInode.lastModifiedTime)) << std::endl;
  if (fileInode.isDirectory ()) {
      std::vector<DirEntry> dirFiles(fileInode.directoryFiles);
      m_dirManager->getDirFiles (fileInode, dirFiles);
      for (size_t i = 0; i < dirFiles.size (); i++) {
          std::cout << dirFiles[i].name << "   ";
      }
      std::cout << std::endl;
    }

  return true;
}

bool FsController::copy(std::string &src, std::string &dst)
{
  std::deque<std::string> srcPath;


  if (!DirManager::parsePath (src, srcPath)) {
      std::cerr << "src file path error" << std::endl;
      return false;
  }

  Inode srcInode;
  if (!getInode(srcPath, srcInode)) {
      return false;
  }

  std::deque<std::string> dstPath;

  if (!DirManager::parsePath (dst, dstPath)) {
      std::cerr << "dst path error" << std::endl;
      return false;
  }

  std::string fileName = dstPath.back ();
  dstPath.pop_back ();

  Inode dstInode;
  if (!getInode(dstPath, dstInode)) {
      return false;
  }

  size_t dstDirId = m_dirManager->getDirFileInodeId (dstInode, fileName);
  if (dstDirId) {
      Inode tmp;
      readInode (tmp, dstDirId);
      if (tmp.isDirectory ()) {
          dstInode = tmp;
          if (!getFileName (srcInode, fileName)) {
            return false;
          }
      }
  }

  if (!dstInode.isDirectory ()) {
      std::cerr << "Error : dst is not a dir" << std::endl;
  }

  copyFile (srcInode, dstInode, fileName);
  dstInode.lastModifiedTime = std::time(NULL);
  dstInode.dataSize += srcInode.dataSize;
  writeInode (dstInode);
  updateSize (dstInode, srcInode.dataSize);
  return true;
}

bool FsController::mkdir(std::string &fs_file)
{
  std::deque<std::string> path;

  if (!DirManager::parsePath (fs_file, path)) {
      std::cerr << "fs file path error" << std::endl;
      return false;
  }
  Inode inode;
  readInode (inode, 1);
  size_t lastExistInodeId = 1;
  while (!path.empty ()) {
      size_t nextInodeId = 0;
      std::string fileName = path.front ();
      nextInodeId = m_dirManager->getDirFileInodeId(inode, fileName);
      if (nextInodeId == 0) {
          break;
      } else {
        readInode (inode, nextInodeId);
      }
      if (inode.isDirectory ()) {
        lastExistInodeId = inode.inodeId;
      } else {
         break;
      }
      path.pop_front ();
  }

  if (path.empty ()) {
      if (fs_file == "/") {
          std::cerr << "incorrect path" << std::endl;
        } else {
          std::cerr << "dir already exists" << std::endl;
        }
      return false;
  }

  Inode lastDir;
  readInode (lastDir, lastExistInodeId);
  while(!path.empty ()) {
       Inode newDir;
       std::string fileName = path.front ();
       path.pop_front ();
       if (!doMkDir (lastDir, newDir, fileName)) {
        return false;
       }
       writeInode (lastDir);
       lastDir = newDir;
  }
  writeInode (lastDir);
  return true;
}



void FsController::unmount()
{
  Inode inode;
  readInode (inode, 1);
  assert(inode.isDirectory());
  //DEBUG
  m_writer.write ((const char*) (&m_superblock), 0, 1, sizeof(Superblock));
  m_writer.writeBitVector (m_dataBitmap.bitmap, 1, m_superblock.dataBitmapBlockNum);
  m_writer.writeBitVector (m_inodeBitmap.bitmap, m_superblock.inodeBitmapStart, m_superblock.inodeBitmapBlockNum);
}

bool FsController::move(std::string &src, std::string &dst)
{
  std::deque<std::string> srcPath;


  if (!DirManager::parsePath (src, srcPath)) {
      std::cerr << "src file path error" << std::endl;
      return false;
  }

  Inode srcInode;
  if (!getInode(srcPath, srcInode)) {
      return false;
  }

  std::deque<std::string> dstPath;

  if (!DirManager::parsePath (dst, dstPath)) {
      std::cerr << "dst path error" << std::endl;
      return false;
  }

  std::string fileName = dstPath.back ();
  dstPath.pop_back ();

  Inode dstInode;
  if (!getInode(dstPath, dstInode)) {
      return false;
  }

  size_t dstDirId = m_dirManager->getDirFileInodeId (dstInode, fileName);
  if (dstDirId) {
      Inode tmp;
      readInode (tmp, dstDirId);
      if (tmp.isDirectory ()) {
          dstInode = tmp;
          if (!getFileName (srcInode, fileName)) {
            return false;
          }
      }
  }

  if (!dstInode.isDirectory ()) {
      std::cerr << "Error : dst is not a dir" << std::endl;
  }

  moveFile (srcInode, dstInode, fileName);

  dstInode.lastModifiedTime = std::time(NULL);
  srcInode.lastModifiedTime = std::time(NULL);
  writeInode (dstInode);
  return true;
}

bool FsController::rm(std::string &file)
{
  std::deque<std::string> srcPath;


  if (!DirManager::parsePath (file, srcPath)) {
      std::cerr << "src file path error" << std::endl;
      return false;
  }

  Inode srcInode;
  if (!getInode(srcPath, srcInode)) {
      return false;
  }

  if (!removeFileFromDir (srcInode)) {
      return false;
    }

  freeResources(srcInode);
  return true;
}

void FsController::updateSize(Inode fileInode, long long dataSize)
{
  while (fileInode.parentId != 0) {
    readInode (fileInode, fileInode.parentId);
    fileInode.dataSize += dataSize;
    writeInode (fileInode);
    }
}

void FsController::freeResources(Inode &srcInode)
{
  if (srcInode.isDirectory ()) {
      std::vector<DirEntry> dirFiles(srcInode.directoryFiles);
      m_dirManager->getDirFiles (srcInode, dirFiles);
      for (size_t i = 2; i < dirFiles.size (); i++) {
          Inode n;
          readInode (n, dirFiles[i].inodeId);
          freeResources (n);
      }
    }
  m_inodeBitmap.setFree (srcInode.inodeId);
  Inode empty;
  empty.inodeId = srcInode.inodeId;
  writeInode (empty);

  freeData(srcInode);

}

void FsController::freeData(Inode &srcInode)
{
  std::vector<BlockRun> srcBlocks;
  m_reader.getBlockRunList (srcInode, srcBlocks);
  BlockListIterator srcIt(srcBlocks);
  while (srcIt.hasNext()) {
    m_dataBitmap.setFree (srcIt.getNext ());
    m_superblock.usedBlocks--;
  }

  if (!srcInode.isSingleBlockRun ()) {
      for (size_t i = srcInode.data.blockId; i < srcInode.data.blockId + srcInode.data.len; i++) {
       m_dataBitmap.setFree (i);
       m_superblock.usedBlocks--;
      }
    }
}

void FsController::copyBlock(size_t from, size_t to, size_t size)
{
  char buffer[m_superblock.blockSize];
  m_reader.read (buffer, from , 1, m_superblock.blockSize, size);
  m_writer.write (buffer, to, 1, m_superblock.blockSize, size);
}

bool FsController::copyData(Inode &srcInode, Inode &copyInode)
{
  assert(!srcInode.isDirectory ());
  std::vector<BlockRun> allocBlocks;
  size_t requiredBlocks = (srcInode.dataSize/(m_superblock.blockSize)) + ((srcInode.dataSize % (m_superblock.blockSize)) ? 1 : 0);

  if (!allocateMemory (allocBlocks, requiredBlocks)) {
      return false;
  }

  if (allocBlocks.size () == 1) {
      copyInode.data.blockId = allocBlocks[0].blockId;
      copyInode.data.len = allocBlocks[0].len;
   } else {
      size_t inderectBlockId = 0;
      if (!m_dataBitmap.getAvaible (inderectBlockId, m_superblock.metaSize)) {
        std::cerr << "Error : not enought memmory for inderect table block" << std::endl;
        return false;
      }
      m_writer.write ((const char*) (&allocBlocks[0]), inderectBlockId, 1, allocBlocks.size () * sizeof(BlockRun));
      m_dataBitmap.setBusy (inderectBlockId);
      m_superblock.usedBlocks += 1;
      copyInode.data.blockId = inderectBlockId;
      copyInode.data.len = 1;
      copyInode.multBlockRunNum = allocBlocks.size ();
    }
  copyInode.dataSize = srcInode.dataSize;

  std::vector<BlockRun> srcBlocks;
  m_reader.getBlockRunList (srcInode, srcBlocks);

  BlockListIterator srcIt(srcBlocks);
  BlockListIterator dstIt(allocBlocks);
  size_t size = srcInode.dataSize;
  while (srcIt.hasNext () && dstIt.hasNext ()) {

    copyBlock (srcIt.getNext (), dstIt.getNext (), size);
    size -= std::min(m_superblock.blockSize, size);
  }
  assert(size == 0);
  assert(!srcIt.hasNext () && !dstIt.hasNext ());
  return true;
}

bool FsController::doMkDir(Inode &dstInode, Inode &newDir, std::string &fileName)
{
  assert(dstInode.isDirectory ());
  if (!m_inodeBitmap.getAvaible (newDir.inodeId)) {
      std::cerr << "Error : no empty inodes" << std::endl;
      return false;
    }

  if (!m_dataBitmap.getAvaible (newDir.data.blockId, m_superblock.metaSize)) {
      std::cerr << "Error : no enought memory for dir meta info" << std::endl;
      return false;
    }
  newDir.data.len = 1;
  m_superblock.usedBlocks++;
  m_dataBitmap.setBusy (newDir.data.blockId);
  m_inodeBitmap.setBusy (newDir.inodeId);
  m_dirManager->writeNewDirInfo (newDir.data.blockId, newDir.inodeId, dstInode.inodeId);

  if (!m_dirManager->addFileToDir (dstInode, newDir, fileName)) {
      return false;
    }
  newDir.lastModifiedTime = std::time(NULL);
  newDir.directoryFiles = 2;
  return true;
}

bool FsController::copyFile(Inode &srcInode, Inode &dstInode, std::string &fileName)
{
  if (srcInode.isDirectory ()) {
      Inode newDir;
      if (!doMkDir(dstInode, newDir, fileName)) {
          return false;
        }
      std::vector<DirEntry> dirFiles(srcInode.directoryFiles);
      m_dirManager->getDirFiles (srcInode, dirFiles);
      for (size_t i = 2; i < dirFiles.size (); i++) {
          Inode n;
          readInode (n, dirFiles[i].inodeId);
          std::string fName(dirFiles[i].name);
          if (!copyFile (n, newDir, fName)) {
              std::cerr << "Error : copy " << dirFiles[i].name << std::endl;
              return false;
            }
      }
      newDir.dataSize = srcInode.dataSize;
      writeInode (newDir);

    } else {
      Inode copyInode;
      if (!m_inodeBitmap.getAvaible (copyInode.inodeId)) {
          std::cerr << "Error : no empty inodes" << std::endl;
          return false;
        }
      if (!copyData(srcInode, copyInode)) {
          return false;
        }
      if (!m_dirManager->addFileToDir (dstInode, copyInode, fileName)) {
          return false;
        }
      m_inodeBitmap.setBusy (copyInode.inodeId);
      copyInode.lastModifiedTime = std::time(NULL);
      writeInode (copyInode);
    }
  return true;
}

bool FsController::moveFile(Inode &srcInode, Inode &dstInode, std::string &fileName)
{

  if (!removeFileFromDir (srcInode)) {
      return false;
    }

  if (!m_dirManager->addFileToDir (dstInode, srcInode, fileName)) {
      return false;
    }

  return true;
}

bool FsController::doExportFile(Inode &fileInode, std::string &host_file)
{
  std::ofstream outputFile;
  outputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  outputFile.open (host_file, std::ios_base::out | std::ios_base::binary);
  std::vector<BlockRun> blockRunsList;
  m_reader.getBlockRunList (fileInode, blockRunsList);

  assert(blockRunsList.size () > 0);
  size_t fileLength = fileInode.dataSize;

  for (size_t i = 0; i < blockRunsList.size (); i++) {
    m_reader.readFromBlockRun (outputFile, blockRunsList[i].blockId, blockRunsList[i].len, fileLength);
  }
  assert(fileLength == 0);
  return true;

}

bool FsController::allocateMemory(std::vector<BlockRun> &allocBlocks, size_t requiredBlocks)
{
  std::list<BlockRun> freeList;
  if (!createFreeList (freeList, requiredBlocks)) {
      auto f = [](BlockRun& b1, BlockRun& b2){return b1.len > b2.len;};
      freeList.sort (f);
      auto it = freeList.begin ();
      while (requiredBlocks > 0) {
          if (it == freeList.end ()) {
              std::cerr << "Error : not enought memmory" << std::endl;
              return false;
          }
          (*it).len = std::min((*it).len, requiredBlocks);
          allocBlocks.push_back (*it);
          ++it;
          requiredBlocks -= std::min((*it).len, requiredBlocks);
      }
    } else {
      BlockRun& block = freeList.back ();
      block.len = std::min(block.len, requiredBlocks);
      allocBlocks.push_back (block);
    }

  for(size_t i = 0; i < allocBlocks.size (); i++) {
      m_dataBitmap.setBusy (allocBlocks[i]);
  }
  m_superblock.usedBlocks += requiredBlocks;

  size_t avaibleInderectSize = m_superblock.blockSize / sizeof(BlockRun);
  if (allocBlocks.size () > avaibleInderectSize) {
      std::cerr << "Error : not enought memmory for inderect table" << std::endl;
      return false;
  }

  return true;

}

bool FsController::createFreeList(std::list<BlockRun> &freeList, size_t requiredBlocks)
{

  std::vector<bool> map = m_dataBitmap.bitmap;
  size_t cnt = 0;
  for (size_t i = m_superblock.metaSize; i < m_superblock.blocksNum; i++) {
      if (!map[i]) {
          cnt++;
          if (cnt == requiredBlocks) {
              freeList.push_back (BlockRun(i + 1 - cnt, cnt));
              return true;
          }
        } else if (cnt != 0){
          freeList.push_back (BlockRun(i - cnt, cnt));
          cnt = 0;
        }
  }
  if (cnt != 0) {
      freeList.push_back (BlockRun(m_superblock.blocksNum - cnt, cnt));
  }
  return false;
}

bool FsController::getInode(std::deque<std::string>& path, Inode &inode)
{
  readInode (inode, 1);

  while (!path.empty ()) {
      std::string fileName = path.front ();
      path.pop_front ();
      size_t nextInodeId = 0;
      if (inode.isDirectory()) {
        nextInodeId = m_dirManager->getDirFileInodeId(inode, fileName);
      }
      if (!inode.isDirectory() || nextInodeId == 0)
      {
        std::cerr << "incorect path" << std::endl;
        return false;
      }
      readInode (inode, nextInodeId);
  }
  return true;
}

void FsController::readSuperblock()
{
  m_reader.read((char *)(&m_superblock), 0, 1, sizeof(Superblock));
}

bool FsController::removeFileFromDir(Inode &srcInode)
{
  Inode dirInode;
  assert(srcInode.parentId != 0);
  readInode (dirInode, srcInode.parentId);
  if (!m_dirManager->removeFileFromDir(dirInode, srcInode)) {
      return false;
    }
  dirInode.dataSize -= srcInode.dataSize;
  long long removeSize = -srcInode.dataSize;
  updateSize (dirInode, removeSize);
  dirInode.lastModifiedTime = std::time(NULL);
  writeInode (dirInode);
  return true;
}

void FsController::readDataBitmap()
{
  m_dataBitmap.init (m_superblock.blocksNum);
  m_reader.readBitVector (m_dataBitmap.bitmap, 1, m_superblock.dataBitmapBlockNum, m_superblock.blocksNum);
}

void FsController::readInodeBitmap()
{
  m_inodeBitmap.init (m_superblock.inodesNum);
  m_reader.readBitVector (m_inodeBitmap.bitmap, m_superblock.inodeBitmapStart, m_superblock.inodeBitmapBlockNum, m_superblock.inodesNum);
}

bool FsController::createRootDir()
{
  size_t inodeId = 1;
  if (!m_inodeBitmap.isFree(inodeId)) {
      std::cerr << "inode bitmap error" << std::endl;
      return false;
    }
  size_t rootBlock = m_superblock.metaSize;
  if (!m_dataBitmap.isFree(rootBlock)) {
      std::cerr << "data bitmap error" << std::endl;
      return false;
    }

  Inode rootInode;
  readInode (rootInode, 1);

  assert(rootInode.inodeId == 1);

  rootInode.data.blockId = rootBlock;
  rootInode.data.len = 1;
  rootInode.directoryFiles = 2;
  rootInode.lastModifiedTime = std::time(NULL);
  writeInode (rootInode);


  m_dirManager->writeNewDirInfo (rootBlock, inodeId, inodeId);

  m_superblock.usedBlocks += 1;
  m_superblock.root = inodeId;
  m_inodeBitmap.setBusy (inodeId);
  m_dataBitmap.setBusy (rootBlock);


  return true;
}

bool FsController::getFileName(Inode &fileInode, std::string& name)
{
  Inode dir;
  readInode (dir, fileInode.parentId);

  assert(dir.isDirectory ());
  std::vector<DirEntry> dirFiles(dir.directoryFiles);
  m_dirManager->getDirFiles (dir, dirFiles);
  for(size_t i = 0; i < dirFiles.size (); i++) {
      if (dirFiles[i].inodeId == fileInode.inodeId) {
          name = std::string(dirFiles[i].name);
          return true;
        }
  }
  return false;

}

bool FsController::importFile(Inode &inode, std::string &filename)
{
  std::ifstream inputFile;
  inputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  inputFile.open (filename, std::ios_base::in | std::ios_base::binary);
  size_t fileLength = getFileLength (inputFile);
  inode.dataSize = fileLength;
  size_t requiredBlocks = (fileLength/(m_superblock.blockSize)) + ((fileLength % (m_superblock.blockSize)) ? 1 : 0);
  if (requiredBlocks > m_superblock.blocksNum - m_superblock.usedBlocks - 1) {
      std::cerr << "Error : not enought memmory (too few empty blocks)" << std::endl;
      return false;
    }
  std::vector<BlockRun> allocBlocks;

  if (!allocateMemory(allocBlocks, requiredBlocks)) {
      return false;
    }


  assert(allocBlocks.size () > 0);
  if (allocBlocks.size () == 1) {
      inode.data.blockId = allocBlocks[0].blockId;
      inode.data.len = allocBlocks[0].len;
      m_writer.writeToBlockRun (inputFile, inode.data.blockId, inode.data.len, fileLength);
    } else {
      size_t inderectBlockId = 0;
      if (!m_dataBitmap.getAvaible (inderectBlockId, m_superblock.metaSize)) {
        std::cerr << "Error : not enought memmory for inderect table block" << std::endl;
        return false;
      }
      m_writer.write ((const char*) (&allocBlocks[0]), inderectBlockId, 1, allocBlocks.size () * sizeof(BlockRun));
      m_dataBitmap.setBusy (inderectBlockId);
      m_superblock.usedBlocks += 1;
      inode.data.blockId = inderectBlockId;
      inode.data.len = 1;
      inode.multBlockRunNum = allocBlocks.size ();
      for (size_t i = 0; i < allocBlocks.size (); i++) {
          m_writer.writeToBlockRun (inputFile, allocBlocks[i].blockId, allocBlocks[i].len, fileLength);
      }

    }
  assert(fileLength == 0);
  inputFile.close ();
  return true;


}

void FsController::writeToBlockRun(std::ifstream& from, size_t startBlock, size_t len, size_t& size) {
  char buffer[m_superblock.blockSize];
  for(size_t block = startBlock; block < startBlock + len; block++) {
      from.read (buffer, std::min(m_superblock.blockSize, size));
      m_writer.write (buffer, block, 1, size);
      size -= std::min(m_superblock.blockSize, size);
    }
}



void FsController::readInode(Inode &inode, size_t inodeId)
{
  size_t inodeTableInd = inodeId / m_superblock.inodesPerBlock;
  size_t offset = inodeId - inodeTableInd * m_superblock.inodesPerBlock;
  m_reader.openBlock (m_superblock.inodeTableStart + inodeTableInd);
  m_reader.setPosInCurrentBlock(offset * sizeof(Inode));
  m_reader.readFromCurrentBlock ((char *) (&inode), sizeof(Inode));
  m_reader.closeCurrentBlock ();
}

void FsController::writeInode(Inode &inode)
{
  size_t inodeTableInd = inode.inodeId / m_superblock.inodesPerBlock;
  size_t offset = inode.inodeId - inodeTableInd * m_superblock.inodesPerBlock;
  Inode* arr = new Inode[m_superblock.inodesPerBlock];
  m_reader.read ((char *) (arr), m_superblock.inodeTableStart + inodeTableInd, 1, m_superblock.inodesPerBlock * sizeof(Inode));
  arr[offset] = inode;
  m_writer.openBlock (m_superblock.inodeTableStart + inodeTableInd);
  m_writer.writeToCurrentBlock ((const char*) (&arr[0]), m_superblock.inodesPerBlock * sizeof(Inode));
  m_writer.closeCurrentBlock ();
  delete arr;

}

size_t FsController::getFileLength(std::ifstream &file)
{
  file.seekg(0, file.end);
  size_t length = file.tellg();
  file.seekg(0, file.beg);

  return length;
}

