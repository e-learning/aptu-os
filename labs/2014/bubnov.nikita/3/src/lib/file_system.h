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

struct BlockHeader
{
	int id;
	int next;
	size_t occupied;
};

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

template <typename T> void log(T const& message)
{
	std::cerr << message << std::endl;
}

struct Config
{
	int blockSize;
	int blockNumber;
};

std::ostream& operator << (std::ostream& os, File const& file);
std::ostream& operator << (std::ostream& os, std::vector<File> const& v);

class FileSystem
{
public:
	FileSystem(std::string const& root, bool isInitialized = true);
	~FileSystem();

	int doInit();
	int doFormat();
	int doImport(std::string const& src,std::string const& dst);
	int doExport(std::string const& dst, std::string const& src);
	int doLs(std::string const& path);
	int doCopy(std::string const& src, std::string const& dst);
	int doMove(std::string const& src, std::string const& dst);
	int doRm(std::string const& name);
	int doMkdir(std::string const& path);

private:
	Config const getConfig() const
	{
		return config;
	}

	template <typename T = File> T getById(int id) const
	{
		T t;
		std::ifstream block(blockName(id), std::ios::binary);
		block.read(reinterpret_cast<char *>(&t), sizeof(T));
		assert(block);
		return t;
	}

	std::string const blockName(int id) const;

	void write(File const& file);
	int format();
	int rm(File const& file);
	int copy(File const& src, File& dst);
	int move(File const& src, File& dst);
	std::vector<File> ls(File const& directory) const;
	File findFile(File& directory, std::string const& name, bool create = true, bool isDirectory = false);
	File findFile(std::string const& dst, bool create = true, bool isDirectory = false);

	File findDirectory(std::vector<std::string> const& path, bool create = true);
	size_t getFreeBlock(size_t const start = 0) const;
	File createFile(std::string const& name, bool isDirectory = true);
	File findDirectory(std::string const& dst, bool create = true);

	void clear(File const& file);

	void remove(int id);
	std::vector<std::string> pathSplit(std::string const &path, char const delimiter);


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
