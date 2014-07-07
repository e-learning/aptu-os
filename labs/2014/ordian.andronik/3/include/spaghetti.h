#ifndef SPAGHETTI_H
#define SPAGHETTI_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <cstring>
#define DEBUG 0

namespace Spaghetti
{

  template <typename T>
  void log(T const& message)
  {
#ifdef DEBUG
    std::cerr << message << std::endl;
#endif
  }

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
    std::time_t lastModified;
  };

  struct Block
  {
    int id;
    int next;
    size_t occupied;
  };

  struct Config
  {
    int blockSize;
    int blockNumber;
  };

  std::ostream & operator<<(std::ostream & os, File const& file)
  {
    os << (file.isDirectory ? "dir" : "file")
       << "\t "
       << file.name
       << "\t";

    if (file.isDirectory)
      os << std::endl;
    else
      os << file.sizeInBlocks
         << " blocks"
         << '\t' << ctime(&file.lastModified);
    return os;
  }


  template<typename T>
  std::ostream & operator<<(std::ostream & os, std::vector<T> const& v)
  {
    os << v.size() << (v.size() == 1 ? " file" : " files") << std::endl;
    for (auto it = v.begin(); it != v.end(); ++it)
      os << *it;
    return os;
  }


  struct FileSystem
  {

    FileSystem(std::string const& root, bool isInitialized = true)
      : rootName(root)
      , initialized(isInitialized)
    {
      std::ifstream configFile(rootName + "/config");
      assert(configFile.good());
      configFile >> config.blockSize >> config.blockNumber;
      assert(configFile && config.blockSize >= 1024);
      blockSize = config.blockSize - sizeof(Block);
      if (initialized)
        {
          rootFile = getById(0);
          occupiedBlocks.clear();
          occupiedBlocks.assign(config.blockNumber, false);
          int currBlockId = 0;
          std::ifstream block(blockName(currBlockId), std::ios::binary);
          block.seekg(sizeof(File));
          for (int i = 0; i != config.blockNumber; )
            {
              char byte;
              block >> byte;
              for (int bit = 0; bit != 8 && i < config.blockNumber; ++bit, ++i)
                occupiedBlocks[i] = byte & (1 << bit);

              if (block.tellg() == config.blockSize)
                {
                  block.close();
                  block.open(blockName(++currBlockId), std::ios::binary);
                }
            }
        }
    }

    ~FileSystem()
    {
      if (initialized)
        {
          size_t currBlockId = 0;
          std::ofstream block(blockName(currBlockId), std::ios::binary | std::ios::in);
          block.write(reinterpret_cast<char *>(&rootFile),
                      sizeof(File));
          assert(block);
          block.seekp(sizeof(File));
          for (int i = 0; i < config.blockNumber;)
            {
              char byte = 0;
              for (int bit = 0; bit != 8 && i < config.blockNumber; ++bit, ++i)
                byte |= occupiedBlocks[i] << bit;

              block << byte;
              assert(block);
              if (block.tellp() == config.blockSize)
                {
                  block.close();
                  block.open(blockName(++currBlockId), std::ios::binary | std::ios::in);
                  assert(block.good());
                }
            }
        }
    }


    int _init()
    {
      std::vector<char> empty(config.blockSize, 0);
      for (int i = 0; i < config.blockNumber; ++i)
        {
          std::string const block = blockName(i);
          std::ofstream os(block, std::ios::binary);
          bool result = os.write(&empty[0], empty.size());
          assert(result);
        }
      return 0;
    }

    int _format()
    {
      return format();
    }

    int _import(std::string const& src,
                std::string const& dst)
    {
      assert(initialized);

      std::ifstream ifs(src, std::ios::binary);
      assert(ifs);

      File file = findFile(dst);
      assert(!file.isDirectory);
      write(file, ifs);
      return 0;
    }

    int _export(std::string const& dst,
                std::string const& src)
    {
      File file = findFile(dst, false);

      std::ofstream ofs(src, std::ios::binary | std::ios::trunc);
      assert(ofs);

      read(file, ofs);
      return 0;
    }

    int _ls(std::string const& path)
    {
      File file = findFile(path, false);
      if (file.isDirectory)
          std::cout << ls(file);
      else
        std::cout << file;
      return 0;
    }

    int _copy(std::string const& src,
              std::string const& dst)
    {
      File source = findFile(src, false);
      File destination = findFile(dst, true, source.isDirectory);

      return copy(source, destination);
    }

    int _move(std::string const& src,
              std::string const& dst)
    {
      File source = findFile(src, false);
      File destination = findFile(dst, true, source.isDirectory);

      return move(source, destination);
    }

    int _rm(std::string const& name)
    {
      File d = findFile(name, false);
      return rm(d);
    }

    int _mkdir(std::string const& path)
    {
      findDirectory(path, true);
      return 0;
    }


  private:

    std::string rootName;
    Config config;
    bool initialized;
    File rootFile;
    size_t blockSize;
    std::vector<bool> occupiedBlocks;


  private:

    Config const getConfig() const
    {
      return config;
    }

    template <typename T = File>
    T getById(int id) const
    {
      T t;
      std::ifstream block(blockName(id), std::ios::binary);
      block.read(reinterpret_cast<char *>(&t), sizeof(T));
      assert(block);
      return t;
    }

    void write(File const& file)
    {
      if (file.id == 0)
        rootFile = file;
      std::ofstream block(blockName(file.id), std::ios::binary | std::ios::in);
      block.write(reinterpret_cast<char const *>(&file),
                  sizeof(File));
      assert(block);
      occupiedBlocks[file.id] = true;
    }

    std::string const blockName(int id) const
    {
      return rootName +"/"+ std::to_string(id);
    }

    int format()
    {
      assert(initialized);
      int fileSize = sizeof(File);
      rootFile = createFile("/");
      rootFile.id = 0;
      occupiedBlocks.assign(config.blockNumber, false);
      int blocks = std::ceil((config.blockNumber / 8.0 + fileSize) / config.blockSize);

      for (int i = 0; i < blocks; ++i)
        occupiedBlocks[i] = true;

      return 0;
    }

    int rm(File const& file)
    {
      if (file.isDirectory)
        {
          if (file.firstChild != -1)
            {
              int nextChild = file.firstChild;
              do
                {
                  File f = getById(nextChild);
                  nextChild = f.next;
                  rm(f);
                } while (nextChild != -1);
            }
        }
      else
        clear(file);

      remove(file.id);
      return 0;
    }

    int copy(File const& src, File & dst)
    {
      assert(!(src.isDirectory && !dst.isDirectory));
      if (src.isDirectory && dst.isDirectory)
        {
          if (src.firstChild != -1)
            {
              int nextChild = src.firstChild;
              do {
                  File child = getById(nextChild);
                  File newFile = findFile(dst, child.name, true, child.isDirectory);
                  copy(child, newFile);
                  nextChild = child.next;
                } while (nextChild != -1);
            }
        }
      else if (!src.isDirectory && dst.isDirectory)
        {
          File newFile = findFile(dst, src.name, true, false);
          copy(src, newFile);
        }
      else if (!src.isDirectory && !dst.isDirectory)
        {
          clear(dst);

          std::vector<char> buffer(blockSize);
          int nextSrcBlock = src.firstBlock;
          int nextDstBlock = dst.firstBlock = getFreeBlock();
          do
            {
              Block block;
              std::ifstream blockFile(blockName(nextSrcBlock), std::ios::binary);
              blockFile.read(reinterpret_cast<char *>(&block), sizeof(Block));
              blockFile.read(&buffer[0], block.occupied);
              blockFile.close();
              nextSrcBlock = block.next;

              Block newBlock = block;
              newBlock.id = nextDstBlock;
              newBlock.next = nextDstBlock = (nextSrcBlock == -1 ? -1 : getFreeBlock(newBlock.id));

              std::ofstream newBlockFile(blockName(newBlock.id), std::ios::binary);
              newBlockFile.write(reinterpret_cast<char *>(&newBlock), sizeof(Block));
              newBlockFile.write(&buffer[0], newBlock.occupied);
              occupiedBlocks[newBlock.id] = true;
            } while (nextSrcBlock != -1);

          dst.sizeInBlocks = src.sizeInBlocks;
          write(dst);
        }
      return 0;
    }

    int move(File const& src, File & dst)
    {
      if (src.isDirectory)
        {
          assert(dst.isDirectory);
          if (src.firstChild != 1)
            {
              File firstChild = getById(src.firstChild);
              if (dst.firstChild == -1)
                {
                  dst.firstChild = firstChild.id;
                  firstChild.parent = dst.id;
                  write(dst);
                }
              else
                {
                  File lastChild = getById(dst.firstChild);
                  while (lastChild.next != -1)
                    lastChild = getById(lastChild.next);
                  lastChild.next = firstChild.id;
                  firstChild.prev = lastChild.id;
                  write(lastChild);
                }
              write(firstChild);
            }
          remove(src.id);
        }
      else
        {
          if (dst.isDirectory)
            dst = findFile(dst, src.name);

          clear(dst);
          dst.sizeInBlocks = src.sizeInBlocks;
          dst.firstBlock = src.firstBlock;
          dst.lastModified = std::time(0);
          write(dst);
          remove(src.id);
        }
      return 0;
    }

    std::vector<File> ls(File const& directory) const
    {
      std::vector<File> files;
      if (directory.firstChild != -1)
        {
          files.push_back(getById(directory.firstChild));
          while (files.back().next != -1)
            files.push_back(getById(files.back().next));
        }
      return files;
    }




    File findFile(File & directory, std::string const& name, bool create = true, bool isDirectory = false)
    {
      if (directory.firstChild == -1)
        {
          if (create)
            {
              File newFile = createFile(name, isDirectory);
              directory.firstChild = newFile.id = getFreeBlock();
              newFile.parent = directory.id;
              write(directory);
              write(newFile);
              return newFile;
            }
        }
      else
        {
          File current = getById(directory.firstChild);
          while (name != current.name && current.next != -1)
            current = getById(current.next);
          if (name == current.name)
            return current;
          else
            {
              if (create)
                {
                  File newFile = createFile(name, isDirectory);
                  current.next = newFile.id = getFreeBlock();
                  newFile.prev = current.id;
                  write(current);
                  write(newFile);
                  return newFile;
                }
            }
        }
      log("File " + name + " not found!");
      assert(false);
    }

    File findDirectory(std::vector<std::string> const& path,
                       bool create = true)
    {
      assert(!path.empty() && path[0] == "");
      File current = rootFile;

      for (auto it = path.begin() + 1; it != path.end(); ++it)
        {
          if (current.firstChild == -1)
            {
              assert(create);
              File newDir = createFile(*it);
              current.firstChild = newDir.id = getFreeBlock();
              newDir.parent = current.id;
              write(current);
              write(newDir);
              current = newDir;
            }
          else
            {
              current = getById(current.firstChild);
              while (*it != current.name && current.next != -1)
                current = getById(current.next);
              if (*it != current.name)
                {
                  assert(create);
                  File newDir = createFile(*it);
                  current.next = newDir.id = getFreeBlock();
                  newDir.prev = current.id;
                  write(current);
                  write(newDir);
                  current = newDir;
                }
              else
                assert(current.isDirectory);
            }
        }
      return current;
    }

    size_t getFreeBlock(size_t const start = 0) const
    {
      for (size_t i = start + 1; i != occupiedBlocks.size(); ++i)
        if (!occupiedBlocks[i])
          return i;
      log("No empty blocks left...");
      assert(false);
    }


    File createFile(std::string const& name,
                    bool isDirectory = true)
    {
      File file;
      file.id = -1;
      file.isDirectory = isDirectory;
      file.prev = -1;
      file.next = -1;
      file.parent = -1;
      file.firstChild = -1;
      file.firstBlock = -1;
      file.sizeInBlocks = 0;
      strncpy(file.name, name.c_str(), sizeof(file.name));
      file.lastModified = std::time(0);
      return file;
    }

    void clear(File const& file)
    {
      if (file.firstBlock != -1)
        {
          Block block = getById<Block>(file.firstBlock);
          while (block.next != -1)
            {
              occupiedBlocks[block.id] = false;
              block = getById<Block>(block.next);
            }
          occupiedBlocks[block.id] = false;
        }
    }

    void remove(int id)
    {
      File file = getById(id);

      if (file.prev == -1)
        {
          if (file.next == -1)
            {
              File parent = getById(file.parent);
              parent.firstChild = -1;
              write(parent);
            }
          else
            {
              File parent = getById(file.parent);
              File next = getById(file.next);
              parent.firstChild = next.id;
              next.parent = parent.id;
              next.prev = -1;
              write(parent);
              write(next);
            }
        }
      else
        {
          if (file.next == -1)
            {
              File prev = getById(file.prev);
              prev.next = -1;
              write(prev);
            }
          else
            {
              File prev = getById(file.prev);
              File next = getById(file.next);
              prev.next = next.id;
              next.prev = prev.id;
              write(prev);
              write(next);
            }
        }

      occupiedBlocks[file.id] = false;
    }


    std::vector<std::string> pathSplit(std::string const &path,
                                       char const delimiter)
    {
      std::stringstream ss(path);
      std::string name;
      std::vector<std::string> directories;
      while(std::getline(ss, name, delimiter))
         directories.push_back(name);
      return directories;
    }



    File findFile(std::string const& dst,
                  bool create = true,
                  bool isDirectory = false)
    {
      std::vector<std::string> path = pathSplit(dst, '/');
      std::string name = path.back();
      path.pop_back();
      if (name == "")
        return rootFile;
      File directory = findDirectory(path, create);
      return findFile(directory, name, create, isDirectory);
    }

    File findDirectory(std::string const& dst, bool create = true)
    {
      return findDirectory(pathSplit(dst, '/'), create);
    }


    void read(File const& file, std::ostream & os)
    {
      std::vector<char> buffer(blockSize);
      int next = file.firstBlock;
      while (next != -1)
        {
          std::ifstream blockFile(blockName(next), std::ios::binary);
          Block block;
          blockFile.read(reinterpret_cast<char *>(&block), sizeof(Block));
          blockFile.read(&buffer[0], block.occupied);
          blockFile.close();
          assert(blockFile);
          os.write(&buffer[0], block.occupied);
          next = block.next;
        }
    }

    void write(File & file, std::istream & is)
    {
      clear(file);
      std::vector<char> buffer(blockSize);
      size_t id = file.firstBlock = getFreeBlock();
      while (is)
        {
          is.read(&buffer[0], blockSize);

          Block block;
          block.id = id;
          block.occupied = is.gcount();
          block.next = is ? getFreeBlock(id) : -1;
          occupiedBlocks[block.id] = true;

          std::ofstream blockFile(blockName(id), std::ios::binary | std::ios::in);
          blockFile.write(reinterpret_cast<char *>(&block), sizeof(Block));
          blockFile.write(&buffer[0], block.occupied);
          blockFile.close();
          assert(blockFile);
          ++file.sizeInBlocks;
          id = block.next;
        }

      write(file);
    }

  };
}
#endif // SPAGHETTI_H
