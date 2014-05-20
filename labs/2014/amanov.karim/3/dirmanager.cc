#include "dirmanager.h"
#include <sstream>
#include <iostream>
#include "fscontroller.h"
#include <assert.h>
#include <list>
#include "fscontroller.h"

const char DirManager::SEPARATOR = '/';

DirManager::DirManager(Superblock &superblock, Bitmap &dataBitmap,
                       Bitmap &inodeBitmap, BlockWriter& w, BlockReader& r)
  : m_superblock(superblock)
  , m_dataBitmap(dataBitmap)
  , m_inodeBitmap(inodeBitmap)
  , m_writer(w)
  , m_reader(r)
{

}

void DirManager::setController(FsController *c)
{
  m_controller = c;
}


void DirManager::writeNewDirInfo(size_t block, size_t id, size_t parent_id)
{
  DirEntry baseEntry[2];
  baseEntry[0].setInodeId (id);
  baseEntry[0].setName (".");
  baseEntry[1].setInodeId (parent_id);
  baseEntry[1].setName ("..");
  m_writer.write ((const char*) (&baseEntry[0]), block, 1, 2 * sizeof(DirEntry));
}

bool DirManager::parsePath(std::string &path, std::deque<std::string> &out)
{

  std::istringstream pathStream(path);
  std::string dirName;
  char root = pathStream.get();
  if (root != SEPARATOR) {
      return false;
  }

  while( std::getline(pathStream , dirName , SEPARATOR) )
  {
    if (dirName.size () > FsController::MAX_NAME_LENGTH) {
        std::cerr << "name length error" << std::endl;
        return false;
    }
    out.push_back (dirName);
  }

  return pathStream.eof();
}

size_t DirManager::getDirFileInodeId(const Inode &dirInode, const std::string &fileName)
{
  assert(dirInode.isDirectory());
  //*********

  std::vector<DirEntry> dirFiles(dirInode.directoryFiles);
  getDirFiles (dirInode, dirFiles);
  for (size_t i = 0; i < dirFiles.size (); i++) {
      if (dirFiles[i].isEqualName(fileName)) {
          return dirFiles[i].inodeId;
      }
  }
  return 0;
}

bool DirManager::addFileToDir(Inode &dirInode, Inode &fileInode, const std::string &fileName)
{
  std::vector<DirEntry> dirFiles(dirInode.directoryFiles);
  std::vector<BlockRun> blockRunsList;
  blockRunsList.push_back (dirInode.data);
  size_t perBlock = (m_superblock.blockSize / sizeof(DirEntry)) * sizeof(DirEntry);
  m_reader.read((char*) (&dirFiles[0]), blockRunsList, perBlock, dirInode.directoryFiles * sizeof(DirEntry));


  for (size_t i = 0; i < dirFiles.size (); i++) {
      if (fileName.compare (dirFiles[i].name) == 0) {
          std::cerr << fileName << " already exist" << std::endl;
          return false;
    }
  }
  size_t avaibleFilesInDir = m_superblock.blockSize / sizeof(DirEntry);
  if (dirInode.directoryFiles == avaibleFilesInDir) {
      std::cerr << "Not enought memory for file in this dir" << std::endl;
      return false;
    }
  fileInode.parentId = dirInode.inodeId;
  DirEntry newFile;
  newFile.setInodeId (fileInode.inodeId);
  newFile.setName (fileName);
  dirFiles.push_back (newFile);
  /*for (size_t i = 0; i < dirFiles.size (); i++) {
      std::cout << dirFiles[i].name << std::endl;
    }*/
  m_writer.write((const char*) (&dirFiles[0]), dirInode.data.blockId, 1, dirFiles.size () * sizeof(DirEntry));
  dirInode.directoryFiles++;
  dirInode.dataSize += fileInode.dataSize;
  m_controller->updateSize(dirInode, fileInode.dataSize);
  return true;
}

bool DirManager::removeFileFromDir(Inode& dirInode, const Inode &inode)
{
  std::vector<DirEntry> dirFiles(dirInode.directoryFiles);
  std::vector<BlockRun> blockRunsList;
  blockRunsList.push_back (dirInode.data);
  size_t perBlock = (m_superblock.blockSize / sizeof(DirEntry)) * sizeof(DirEntry);
  m_reader.read((char*) (&dirFiles[0]), blockRunsList, perBlock, dirInode.directoryFiles * sizeof(DirEntry));

  for(size_t i = 0; i < dirFiles.size (); i++) {
      if (dirFiles[i].inodeId == inode.inodeId) {
          dirFiles.erase (dirFiles.begin () + i);
          break;
        }
    }
  dirInode.directoryFiles--;

  m_writer.write((const char*) (&dirFiles[0]), dirInode.data.blockId, 1, dirFiles.size () * sizeof(DirEntry));
  return true;


}

void DirManager::getDirFiles(const Inode &dirInode, std::vector<DirEntry> &dirFiles)
{
  std::vector<BlockRun> blockRunsList;
  size_t filesNum = dirInode.directoryFiles;
  m_reader.getBlockRunList (dirInode, blockRunsList);

  size_t perBlock = (m_superblock.blockSize / sizeof(DirEntry)) * sizeof(DirEntry);
  m_reader.read((char*) (&dirFiles[0]), blockRunsList, perBlock, filesNum * sizeof(DirEntry));

}

