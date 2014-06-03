#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cmath>
#include <cassert>
#include <cstring>
#define DEBUG

#include "File.h"
#include "BlockHeader.h"

template <typename T>
void log(T const& message)
{
#ifdef DEBUG
	std::cerr << message << std::endl;
#endif
}

struct Config
{
	int blockSize;
	int blockNumber;
};

std::ostream& operator<<(std::ostream& os, File const& file);
std::ostream& operator<<(std::ostream& os, std::vector<File> const& v);

class FileSystem
{
public:
	FileSystem(std::string const& root, bool isInitialized = true);
	~FileSystem();

	int doInit()
	{
		std::vector<char> empty(config.blockSize, 0);
		for (int i = 0; i < config.blockNumber; ++i)
			{
				std::string const block = blockName(i);
				std::ofstream ofs(block, std::ios::binary);
				assert(ofs.write(&empty[0], empty.size()));
			}
		return 0;
	}

	int doFormat()
	{
		return format();
	}

	int doImport(std::string const& src,
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

	int doExport(std::string const& dst,
							std::string const& src)
	{
		File file = findFile(dst, false);

		std::ofstream ofs(src, std::ios::binary | std::ios::trunc);
		assert(ofs);

		read(file, ofs);
		return 0;
	}

	int doLs(std::string const& path)
	{
		File file = findFile(path, false);
		if (file.isDirectory)
			std::cout << ls(file);
		else
			std::cout << file;
		return 0;
	}

	int doCopy(std::string const& src,
						std::string const& dst)
	{
		File source = findFile(src, false);
		File destination = findFile(dst, true, source.isDirectory);

		return copy(source, destination);
	}

	int doMove(std::string const& src,
						std::string const& dst)
	{
		File source = findFile(src, false);
		File destination = findFile(dst, true, source.isDirectory);

		return move(source, destination);
	}

	int doRm(std::string const& name)
	{
		File d = findFile(name, false);
		return rm(d);
	}

	int doMkdir(std::string const& path)
	{
		findDirectory(path, true);
		return 0;
	}

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
		block.write(reinterpret_cast<char const*>(&file),
								sizeof(File));
		assert(block);
		occupiedBlocks[file.id] = true;
	}

	std::string const blockName(int id) const
	{
		return rootName + "/" + std::to_string(id);
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
		

	int rm(File const& file);

	int copy(File const& src, File& dst);

	int move(File const& src, File& dst);

	std::vector<File> ls(File const& directory) const;

	File findFile(File& directory, std::string const& name, bool create = true, bool isDirectory = false);

	File findDirectory(
			std::vector<std::string> const& path,
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

	size_t getFreeBlock(size_t const start = 0) const;

	File createFile(
			std::string const& name,
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

	void clear(File const& file);

	void remove(int id);
	std::vector<std::string> pathSplit(
			std::string const &path,
			char const delimiter);

	File findFile(
			std::string const& dst,
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
	

	File findDirectory(std::string const& dst, bool create = true);


	void read(File const& file, std::ostream& os);

	void write(File& file, std::istream& ifs);

	std::string rootName;
	Config config;
	bool initialized;
	File rootFile;
	size_t blockSize;
	std::vector<bool> occupiedBlocks;
};

#endif
