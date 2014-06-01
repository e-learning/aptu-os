#include "FileSystem.h"

FileSystem::FileSystem(std::string const& root, bool isInitialized)
	: rootName(root)
	, initialized(isInitialized)
{
	std::ifstream configFile(rootName + "/config");
	assert(configFile.good());
	configFile >> config.blockSize >> config.blockNumber;
	assert(configFile && config.blockSize >= 1024);
	blockSize = config.blockSize - sizeof(BlockHeader);
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

FileSystem::~FileSystem()
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

int FileSystem::rm(File const& file)
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

int FileSystem::copy(File const& src, File& dst)
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
					BlockHeader block;
					std::ifstream blockFile(blockName(nextSrcBlock), std::ios::binary);
					blockFile.read(reinterpret_cast<char *>(&block), sizeof(BlockHeader));
					blockFile.read(&buffer[0], block.occupied);
					blockFile.close();
					nextSrcBlock = block.next;

					BlockHeader newBlock = block;
					newBlock.id = nextDstBlock;
					newBlock.next = nextDstBlock = (nextSrcBlock == -1 ? -1 : getFreeBlock(newBlock.id));

					std::ofstream newBlockFile(blockName(newBlock.id), std::ios::binary);
					newBlockFile.write(reinterpret_cast<char *>(&newBlock), sizeof(BlockHeader));
					newBlockFile.write(&buffer[0], newBlock.occupied);
					occupiedBlocks[newBlock.id] = true;
				} while (nextSrcBlock != -1);

			dst.sizeInBlocks = src.sizeInBlocks;
			write(dst);
		}
	return 0;
}

int FileSystem::move(File const& src, File& dst)
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

std::vector<File> FileSystem::ls(File const& directory) const
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

File FileSystem::findFile(File& directory, std::string const& name, bool create, bool isDirectory)
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

size_t FileSystem::getFreeBlock(size_t const start) const
{
	for (size_t i = start + 1; i != occupiedBlocks.size(); ++i)
		if (!occupiedBlocks[i])
			return i;
	log("No empty blocks left...");
	assert(false);
}

void FileSystem::clear(File const& file)
{
	if (file.firstBlock != -1)
		{
			BlockHeader block = getById<BlockHeader>(file.firstBlock);
			while (block.next != -1)
				{
					occupiedBlocks[block.id] = false;
					block = getById<BlockHeader>(block.next);
				}
			occupiedBlocks[block.id] = false;
		}
}

void FileSystem::remove(int id)
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

std::vector<std::string> FileSystem::pathSplit(
		std::string const &path,
		char const delimiter)
{
	std::stringstream ss(path);
	std::string name;
	std::vector<std::string> directories;
	while(std::getline(ss, name, delimiter))
		 directories.push_back(name);
	return directories;
}

File FileSystem::findDirectory(std::string const& dst, bool create)
{
	return findDirectory(pathSplit(dst, '/'), create);
}


void FileSystem::read(File const& file, std::ostream& os)
{
	std::vector<char> buffer(blockSize);
	int next = file.firstBlock;
	while (next != -1)
		{
			std::ifstream blockFile(blockName(next), std::ios::binary);
			BlockHeader block;
			blockFile.read(reinterpret_cast<char*>(&block), sizeof(BlockHeader));
			blockFile.read(&buffer[0], block.occupied);
			blockFile.close();
			assert(blockFile);
			os.write(&buffer[0], block.occupied);
			next = block.next;
		}
}

void FileSystem::write(File& file, std::istream& ifs)
{
	clear(file);
	std::vector<char> buffer(blockSize);
	size_t id = file.firstBlock = getFreeBlock();
	while (ifs)
		{
			ifs.read(&buffer[0], blockSize);

			BlockHeader block;
			block.id = id;
			block.occupied = ifs.gcount();
			block.next = ifs ? getFreeBlock(id) : -1;
			occupiedBlocks[block.id] = true;

			std::ofstream blockFile(blockName(id), std::ios::binary | std::ios::in);
			blockFile.write(reinterpret_cast<char*>(&block), sizeof(BlockHeader));
			blockFile.write(&buffer[0], block.occupied);
			blockFile.close();
			assert(blockFile);
			++file.sizeInBlocks;
			id = block.next;
		}
	write(file);
}
