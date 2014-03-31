#include <fstream>
#include <vector>
#include <iostream>
#include "FileSystem.hpp"

using std::runtime_error;
using std::string;
using std::ofstream;
using std::ifstream;

const string FileSystem::FAT_FILENAME = "fat.txt";
const string FileSystem::CONFIG_FILENAME = "config";
const string FileSystem::MAP_FILENAME = "map.txt";

FileSystem::FileSystem(string const &root) : location(utils::pathAppend(root))
{
    read_config();
}

void FileSystem::read_config()
{
    std::ifstream config_file(utils::pathAppend(location, CONFIG_FILENAME));
    if (!config_file.is_open())
        throw runtime_error("Error occurs while reading the CONFIG file");
    config_file >> blockSize >> blocksCount;
    if (blocksCount <= 0 || blockSize < 1024)
        throw runtime_error("Config file found, but has the wrong configuration");
}

void FileSystem::run_init()
{
    std::vector<char> zeroes(blockSize, 0);
    for (size_t i = 0; i < blocksCount; i++) {
        ofstream block(utils::pathAppend(location, std::to_string(i)));
        block.write(zeroes.data(), zeroes.size());
    }
}

void FileSystem::run_format()
{
    run_init();
    root = Directory("/");
    usedBlocks.assign(blocksCount, 0);
}

void FileSystem::save()
{
    ofstream fat(utils::pathAppend(location, FAT_FILENAME), std::ios_base::trunc);
    ofstream map(utils::pathAppend(location, MAP_FILENAME),
                 std::ios_base::trunc | std::ios_base::binary);
    if (good()) {
        root.save(fat);
    }
    copy(usedBlocks.begin(), usedBlocks.end(), std::ostream_iterator<char>(map));
}

void FileSystem::load()
{
    ifstream fat(utils::pathAppend(location, FAT_FILENAME));
    ifstream map(utils::pathAppend(location, MAP_FILENAME), std::ios_base::binary);
    usedBlocks.assign(std::istream_iterator<char>(map), std::istream_iterator<char>());
    root.load(fat);
}

void FileSystem::run_import(const string &host_fileName, string const &fs_filename)
{
    ifstream hostFileStream(host_fileName, std::ios_base::ate | std::ios_base::binary);
    if (!hostFileStream.is_open())
        throw runtime_error("Host file not found");

    size_t host_fileSize = hostFileStream.tellg();
    size_t host_fileBlockSize = host_fileSize / blockSize + (host_fileSize % blockSize != 0);
    
    if (host_fileBlockSize > getFreeBlocksCount())
        throw runtime_error("Not enought space");
    hostFileStream.seekg(0);

    Path path(fs_filename);
    if (exists(path))
        throw runtime_error("File with the name '" + fs_filename + "' already exists");

    if (!isDirectory(*path.getParentPath()))
        throw runtime_error("Path to file with the name '" + fs_filename + "' not found");

    File file(path.getFileName(), host_fileSize);
    while (hostFileStream.tellg() != -1) {
        size_t block = getNextFreeBlock();
        utils::write_to_block(
            hostFileStream, utils::pathAppend(location, std::to_string(block)), blockSize);
        file.addUsedBlock(block);
        usedBlocks[block] = 1;
    }
    root.findLastDirectory(path)->addFile(file);
}

void FileSystem::run_export(const string &fs_filename, const string &host_filename)
{
    ofstream hostFileStream(host_filename, std::ios_base::trunc | std::ios_base::binary);
    if (!hostFileStream.is_open())
        throw runtime_error("Host file cannot be created or opened");

    Path path(fs_filename);
    if (!exists(path) || !isFile(path))
        throw runtime_error("File '" + path.getFileName() + "' not found");

    File const &file = root.findLastDirectory(path)->getFile(path.getFileName());

    for (auto block : file.getBlocks()) {
        utils::read_from_block(
            hostFileStream, utils::pathAppend(location, std::to_string(block)), blockSize);
    }
}

void FileSystem::run_ls(const string &fileName)
{
    if (fileName == root.getName()) {
        std::cout << root.get_info() << std::endl;
        return;
    }

    Path path(fileName);
    if (!exists(path))
        throw runtime_error("File " + fileName + " not found");

    if (isDirectory(path)) {
        std::cout << root.findDirectory(path)->get_info() << std::endl;
    } else {
        std::cout << root.findFile(path)->get_info() << std::endl;
    }
}

void FileSystem::run_mkdir(const string &directory)
{
    Path path(directory);
    Directory *currentDir = &root;

    for (string const &name : path.getSplittedPath()) {
        if (currentDir->existsFile(name))
            throw runtime_error("File '" + name + "' exists in directory " + currentDir->getName());

        if (!currentDir->existsDir(name)) {
            Directory d(name);
            currentDir->addDirectory(d);
        }
        currentDir = &currentDir->getSubDir(name);
    }
}

void FileSystem::run_rm(string const &filename)
{
    Path path(filename);
    if (!exists(path))
        throw runtime_error("File '" + filename + "' not found");

    Directory *targetDirectory = root.findLastDirectory(path);
    if (isFile(path)) {
        remove_file(*targetDirectory, targetDirectory->getFile(path.getFileName()));
    } else {
        remove_dir(*targetDirectory, targetDirectory->getSubDir(path.getFileName()));
        return;
    }
}

void FileSystem::run_copy(string const &source, string const &destination)
{
    Path pathSource(source);

    if (!exists(pathSource))
        throw runtime_error("File " + source + " not found");

    Directory *sourceParentDir = root.findLastDirectory(pathSource);
    if (isFile(pathSource))
        copy_file_to(sourceParentDir->getFile(pathSource.getFileName()), destination);
    else
        copy_dir_to(sourceParentDir->getSubDir(pathSource.getFileName()), destination);
}

void FileSystem::run_move(string const &source, string const &destination)
{
    Path pathSource(source);

    if (!exists(pathSource))
        throw runtime_error("File " + source + " not found");

    Directory *sourceParentDir = root.findLastDirectory(pathSource);
    if (isFile(pathSource))
        move_file_to(
            sourceParentDir->getFile(pathSource.getFileName()), sourceParentDir, destination);
    else
        throw runtime_error("Cannot move dir");
}

void FileSystem::remove_file(Directory &dir, File &f)
{
    for (auto block : f.getBlocks()) {
        usedBlocks[block] = 0;
    }
    dir.removeFile(f);
}

void FileSystem::remove_dir(Directory &dir, Directory &target)
{
    for (auto t : target.getAllDirectories()) {
        remove_dir(target, t);
    }
    for (auto f : target.getAllFiles()) {
        remove_file(target, f);
    }
    dir.removeDir(target);
}

void FileSystem::copy_dir_to(Directory const &directory, const string &destination)
{
    if (destination == "/") {
        copy_directory(directory, root);
        return;
    }

    Path path(destination);
    if (!exists(*path.getParentPath()))
        throw runtime_error("Path to '" + destination + "' not found");
    if (isFile(path))
        throw runtime_error("Can't copy directory to file");

    Directory *targetDirectory = root.findLastDirectory(path);
    if (!isDirectory(path)) {
        Directory d(directory.getName());
        targetDirectory->addDirectory(d);
    }
    copy_directory(directory, targetDirectory->getSubDir(directory.getName()));
}

void FileSystem::copy_file_to(File const &file, const string &destination)
{
    if (destination == "/") {
        copy_file(file, root, file.getName());
        return;
    }

    Path path(destination);
    if (!exists(*path.getParentPath()))
        throw runtime_error("Path to '" + destination + "' not found");

    if (isDirectory(path)) {
        if (root.findDirectory(path)->existsFile(file.getName())) {
            if (root.findDirectory(path)->getFile(file.getName()) == file)
                return;
        }
        copy_file(file, *root.findDirectory(path), file.getName());
    } else {
        if (isFile(path) && *root.findFile(path) == file)
            return;
        copy_file(file, *root.findLastDirectory(path), path.getFileName());
    }
}

void FileSystem::move_file_to(File const &file, Directory *parentDir, const string &destination)
{
    if (destination == "/") {
        move_file(file, parentDir, root, file.getName());
        return;
    }

    Path path(destination);
    if (!exists(*path.getParentPath()))
        throw runtime_error("Path to '" + destination + "' not found");

    if (isDirectory(path)) {
        if (root.findDirectory(path)->existsFile(file.getName())) {
            if (root.findDirectory(path)->getFile(file.getName()) == file)
                return;
        }
        move_file(file, parentDir, *root.findDirectory(path), file.getName());
    } else {
        if (isFile(path) && *root.findFile(path) == file)
            return;
        move_file(file, parentDir, *root.findLastDirectory(path), path.getFileName());
    }
}

void FileSystem::copy_file(File const &file, Directory &targetDirectory, string const &newName)
{
    if (targetDirectory.existsFile(newName))
        remove_file(targetDirectory, targetDirectory.getFile(file.getName()));

    if (targetDirectory.existsDir(file.getName()))
        throw runtime_error("Cannot overwrite directory '" + file.getName() +
                            " with non-directory");

    if (file.getBlocks().size() > getFreeBlocksCount())
        throw runtime_error("Not enought space");

    File newFile(newName, file.getSize());
    for (size_t i = 0; i < file.getBlocks().size(); ++i) {
        size_t block = getNextFreeBlock();
        utils::write_to_block(utils::pathAppend(location, std::to_string(file.getBlocks()[i])),
                              utils::pathAppend(location, std::to_string(block)),
                              blockSize);
        newFile.addUsedBlock(block);
        usedBlocks[block] = 1;
    }
    targetDirectory.addFile(newFile);
}

void FileSystem::move_file(File const &file,
                           Directory *parentDir,
                           Directory &targetDirectory,
                           string const &newName)
{
    File f(file);
    f.setName(newName);
    targetDirectory.addFile(f);
    parentDir->removeFile(parentDir->getFile(file.getName()));
}

void FileSystem::copy_directory(Directory const &directory, Directory &targetDirectory)
{
    for (auto d : directory.getAllDirectories()) {
        copy_directory(d, targetDirectory);
    }
    for (auto f : directory.getAllFiles()) {
        copy_file(f, targetDirectory, f.getName());
    }
}

bool FileSystem::exists(Path const &path)
{
    Directory *d = root.findLastDirectory(path);
    return d == nullptr ? false
                        : d->existsFile(path.getFileName()) || d->existsDir(path.getFileName());
}

bool FileSystem::isDirectory(Path const &path)
{
    Directory *d = root.findLastDirectory(path);
    return d == nullptr ? false : d->existsDir(path.getFileName());
}

bool FileSystem::isFile(Path const &path)
{
    Directory *d = root.findLastDirectory(path);
    return d == nullptr ? false : d->existsFile(path.getFileName());
}