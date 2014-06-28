#include "filesystem.h"
#include <math.h>
#include <cstring>
#include <string>
#include <sstream>

using std::string;

FileSystem::FileSystem(const char *root)
    : root(root),
      config(readConfig()),
      blockSize(config.blockSize - sizeof(BlockEntry)),
      isInitialised(ifstream(getBlockById(0), ios::binary)) {

    if (isInitialised) {
        rootDir = readEntry(0);
        readBlockMap();
    }
}

FileSystem::~FileSystem() {
    if (isInitialised) {
        writeBlockMap();
    }
}

void FileSystem::format() {
    checkInit();

    int rootDirSize = sizeof(FileEntry);
    rootDir = initEntry("/");
    rootDir.id = 0;

    freeBlocks.assign(config.numberOfBlocks, false);
    int block_map_blocks = ceil((config.numberOfBlocks / 8.0 + rootDirSize) / config.blockSize);
    for (int i = 0; i < block_map_blocks; ++i) {
        freeBlocks[i] = true;
    }
}

void FileSystem::rm(const FileEntry &entry) {
    if (entry.dir) {
        if (entry.firstChild != -1) {
            int nextChild = entry.firstChild;
            do {
                FileEntry c = readEntry(nextChild);
                rm(c);
                nextChild = c.nextFile;
            } while (nextChild != -1);
        }
    } else {
        clearEntry(entry);
    }
    rmEntry(entry.id);
}

vector<FileEntry> FileSystem::list(const FileEntry & directory) const {
    vector<FileEntry> entries;
    if (directory.firstChild != -1) {
        entries.push_back(readEntry(directory.firstChild));
        while (entries.back().nextFile != -1) {
            entries.push_back(readEntry(entries.back().nextFile));
        }
    }
    return entries;
}

FileEntry FileSystem::initEntry(const string &name, bool isDir) {
    FileEntry entry;
    entry.id = -1;
    entry.dir = isDir;
    entry.firstChild = -1;
    entry.parentFile = -1;
    entry.nextFile = -1;
    entry.previousFile = -1;
    entry.firstBlock = -1;
    entry.nBlocks = 0;
    strncpy(entry.name, name.c_str(), sizeof(entry.name));
    entry.time = time(0);
    return entry;
}

void FileSystem::clearEntry(const FileEntry &fd) {
    if (fd.firstBlock != -1) {
        BlockEntry bd = readEntry<BlockEntry>(fd.firstBlock);
        while (bd.next != -1) {
            freeBlocks[bd.id] = false;
            bd = readEntry<BlockEntry>(bd.next);
        }
        freeBlocks[bd.id] = false;
    }
}

void FileSystem::rmEntry(int const id) {

    FileEntry entry = readEntry(id);

    if (entry.previousFile == -1) {

        if (entry.nextFile == -1) {

            FileEntry parent = readEntry(entry.parentFile);
            parent.firstChild = -1;
            writeEntry(parent);

        } else {

            FileEntry parent = readEntry(entry.parentFile);
            FileEntry next = readEntry(entry.nextFile);
            parent.firstChild = next.id;
            next.parentFile = parent.id;
            next.previousFile = -1;
            writeEntry(parent);
            writeEntry(next);

        }
    } else {

        if (entry.nextFile == -1) {
            FileEntry prev = readEntry(entry.previousFile);
            prev.nextFile = -1;
            writeEntry(prev);

        } else {

            FileEntry prev = readEntry(entry.previousFile);
            FileEntry next = readEntry(entry.nextFile);
            prev.nextFile = next.id;
            next.previousFile = prev.id;
            writeEntry(prev);
            writeEntry(next);

        }
    }

    freeBlocks[entry.id] = false;
}

FileEntry FileSystem::findEntry(FileEntry &directory, const string &entryName, bool doCreate, bool isDirectory) {
    if (directory.firstChild == -1) {

        if (doCreate) {

            FileEntry newEntry = initEntry(entryName, isDirectory);
            directory.firstChild = newEntry.id = getFirstAvailableBlock();
            newEntry.parentFile = directory.id;
            writeEntry(directory);
            writeEntry(newEntry);
            return newEntry;

        }
    } else {

        FileEntry current = readEntry(directory.firstChild);
        while (entryName != current.name && current.nextFile != -1) {

            current = readEntry(current.nextFile);

        }
        if (entryName == current.name) {

            return current;

        } else {

            if (doCreate) {

                FileEntry newFile = initEntry(entryName, isDirectory);
                current.nextFile = newFile.id = getFirstAvailableBlock();
                newFile.previousFile = current.id;
                writeEntry(current);
                writeEntry(newFile);
                return newFile;

            }
        }
    }
    throw "Cannot find '" + entryName + "'";
}
std::vector<std::string> splitStringBy(const std::string &str, const char delimiter) {
    std::stringstream test(str);
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(test, segment, delimiter)) {
        seglist.push_back(segment);
    }

    return seglist;
}

FileEntry FileSystem::findEntry(const char *destination, bool doCreate, bool isDir) {
    vector<string> path = splitStringBy(string(destination), '/');
    string entryName = path.back(); path.pop_back();
    if (entryName == "") {
        return rootDir;
    }
    FileEntry directory = findDir(path, doCreate);
    return findEntry(directory, entryName, doCreate, isDir);
}

FileEntry FileSystem::findDir(const char *destination, bool create) {
    return findDir(splitStringBy(destination, '/'), create);
}

FileEntry FileSystem::findDir(const vector<string> &path, bool create) {
    if (path.size() < 1 || path[0] != "") {
        throw "Path should start with /";
    }

    FileEntry curEntry = rootDir;

    for (auto it = path.begin() + 1; it != path.end(); ++it) {

        if (curEntry.firstChild == -1) {
            if (create) {
                FileEntry new_directory = initEntry(*it);
                curEntry.firstChild = new_directory.id = getFirstAvailableBlock();
                new_directory.parentFile = curEntry.id;
                writeEntry(curEntry);
                writeEntry(new_directory);
                curEntry = new_directory;

            } else {
                throw "Cannot find '" + *it + "'";
            }
        } else {
                    //traverse dir
            curEntry = readEntry(curEntry.firstChild);
            while (*it != curEntry.name && curEntry.nextFile != -1) {
                curEntry = readEntry(curEntry.nextFile);
            }
            if (*it == curEntry.name) {

                if (!curEntry.dir) {
                    throw "'" + *it + "' is a file";
                }
            } else {
                if (create) {
                    FileEntry new_directory = initEntry(*it);
                    curEntry.nextFile = new_directory.id = getFirstAvailableBlock();
                    new_directory.previousFile = curEntry.id;
                    writeEntry(curEntry);
                    writeEntry(new_directory);
                    curEntry = new_directory;
                } else {
                    throw "Cannot find '" + *it + "'";
                }
            }
        }
    }

    return curEntry;
}

void FileSystem::readData(FileEntry const &fd, std::ostream &destinationStream) {
    char * buf = new char[blockSize];
    int next = fd.firstBlock;

    while (next != -1) {
        ifstream block(getBlockById(next), ios::binary);
        BlockEntry blockEntry;
        block.read((char *) &blockEntry, sizeof(BlockEntry));
        block.read(buf, blockEntry.length);
        block.close();
        if (!block) {
            delete[] buf;
            throw "Cannot read block";
        }

        destinationStream.write(buf, blockEntry.length);
        next = blockEntry.next;
    }
    delete[] buf;
}

void FileSystem::writeData(FileEntry &entry, std::istream &sourceStream) {

    clearEntry(entry);

    char * buf = new char[blockSize];
    size_t id = entry.firstBlock = getFirstAvailableBlock();
    while (sourceStream) {
        sourceStream.read(buf, blockSize);

        BlockEntry blockEntry;
        blockEntry.id = id;
        blockEntry.length = sourceStream.gcount();
        blockEntry.next = sourceStream ? getFirstAvailableBlock(id) : -1;
        freeBlocks[blockEntry.id] = true;

        ofstream block(getBlockById(id), ios::binary | ios::trunc);
        block.write((char *) &blockEntry, sizeof(BlockEntry));
        block.write(buf, blockEntry.length);
        block.close();
        if (!block) {
            throw "Cannot write block";
        }

        entry.nBlocks++;
        id = blockEntry.next;
    }

    writeEntry(entry);
}

void FileSystem::readBlockMap() {
    freeBlocks.clear();
    freeBlocks.assign(config.numberOfBlocks, false);
    int currentBlockId = 0;
    ifstream block(getBlockById(currentBlockId), ios::in|ios::binary);
    block.seekg(sizeof(FileEntry));
    for (int i = 0; i < config.numberOfBlocks; ){
        char byte;
        block >> byte;
        for (int b = 0; b < 8 && i < config.numberOfBlocks; b++,i++) {
            freeBlocks[i] = byte & (1 << b);
        }
        if (block.tellg() == config.blockSize){
            block.close();
            block.open(getBlockById(++currentBlockId), ios::binary);
        }
    }
}

void FileSystem::writeBlockMap() {
    int currentBlockId = 0;
    ofstream block(getBlockById(currentBlockId), ios::binary|ios::trunc);
    block.write((char *)&rootDir, sizeof(FileEntry));
    if (!block) {
        throw "Can't open block";
    }
    block.seekp(sizeof(FileEntry));
    for (int i = 0; i < config.numberOfBlocks;) {
        char byte = 0;
        for (int b = 0; b < 8 && i < config.numberOfBlocks; b++,i++) {
            byte |= freeBlocks[i] << b;
        }
        block << byte;
        if (!block) {
            throw "Write error";
        }
        if (block.tellp() == config.blockSize){
            block.close();
            block.open(getBlockById(++currentBlockId), ios::binary);
            if (!block) {
                throw "Cannot open block";
            }
        }
    }
    block.close();
}

void FileSystem::copy(const FileEntry &from, FileEntry &to) {

    if (from.dir && !to.dir) {
        throw (string("'") + to.name + "' is a file");
    } else if (from.dir && to.dir) {

        if (from.firstChild != -1) {
            int nextChild = from.firstChild;
            while (nextChild != -1) {
                FileEntry child = readEntry(nextChild);
                FileEntry newEntry = findEntry(to, child.name, true, child.dir);
                copy(child, newEntry);
                nextChild = child.nextFile;
            }
        }

    } else  if (!from.dir && to.dir) {
        FileEntry newFileEntry = findEntry(to, from.name, true, false);
        copy(from, newFileEntry);
    } else if (!from.dir && !to.dir) {
        clearEntry(to);

        char * buf = new char[blockSize];
        int nextSrc = from.firstBlock;
        int nextDestination = to.firstBlock = getFirstAvailableBlock();
        while (nextSrc != -1) {
            BlockEntry blockEntry;
            ifstream block(getBlockById(nextSrc), ios::binary);
            block.read((char *) &blockEntry, sizeof(BlockEntry));
            block.read(buf, blockEntry.length);
            block.close();
            nextSrc = blockEntry.next;

            BlockEntry newBlockEntry = blockEntry;
            newBlockEntry.id = nextDestination;
            newBlockEntry.next = nextDestination = (nextSrc == -1 ? -1 : getFirstAvailableBlock(newBlockEntry.id));

            ofstream new_block(getBlockById(newBlockEntry.id), ios::binary);
            new_block.write((char *)&newBlockEntry, sizeof(BlockEntry));
            new_block.write(buf, newBlockEntry.length);
            freeBlocks[newBlockEntry.id] = true;
        }

        to.nBlocks = from.nBlocks;
        writeEntry(to);
    }
}

void FileSystem::move(const FileEntry &sourceEntry, FileEntry &destEntry) {
    if (sourceEntry.dir) {
        if (destEntry.dir) {
            if (sourceEntry.firstChild != 1) {
                FileEntry firstChild = readEntry(sourceEntry.firstChild);
                if (destEntry.firstChild == -1) {
                    destEntry.firstChild = firstChild.id;
                    firstChild.parentFile = destEntry.id;
                    writeEntry(destEntry);
                } else {
                    FileEntry lastChild = readEntry(destEntry.firstChild);
                    while (lastChild.nextFile != -1) {
                        lastChild = readEntry(lastChild.nextFile);
                    }
                    lastChild.nextFile = firstChild.id;
                    firstChild.previousFile = lastChild.id;
                    writeEntry(lastChild);
                }
                writeEntry(firstChild);
            }
            rmEntry(sourceEntry.id);
        } else {
            throw string("'") + destEntry.name + "' is a file. Cannot move";
        }
    } else {
        if (destEntry.dir) {
            destEntry = findEntry(destEntry, sourceEntry.name);
        }
        clearEntry(destEntry);
        destEntry.nBlocks = sourceEntry.nBlocks;
        destEntry.firstBlock = sourceEntry.firstBlock;
        destEntry.time = time(0);
        writeEntry(destEntry);
        rmEntry(sourceEntry.id);
    }
}

ConfigInfo FileSystem::readConfig() {
    int MIN_BLOCK_SIZE = 1024;
    int MIN_N_BLOCKS = 1;
    ConfigInfo config;
    std::ifstream confFile((string(root) + "/config").c_str());
    if (!confFile.good()) {
        throw "Cannot open config file";
    }
    confFile >> config.blockSize >> config.numberOfBlocks;
    if (!confFile.good()
            || config.blockSize < MIN_BLOCK_SIZE || config.numberOfBlocks < MIN_N_BLOCKS) {
        throw "Wrong config info";
    }
    return config;
}
