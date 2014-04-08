#include "fsmanager.h"

#include <fstream>
#include <iostream>

#include "dir.h"
#include "file.h"
#include "blocklist.h"

using std::ifstream;
using std::ofstream;
using std::string;

void FSManager::Init(const string &root) {
    for(size_t i = 0; i != memmanager_.BlocksNum(); ++i) {
        ofstream ofs(string(root + '/' + std::to_string(i)));
        ofs.close();
    }
}

bool FSManager::Format(const string &root) {
    return (memmanager_.FreeSpace() > Dir::EMPDIR_SIZE_BLOCKS) &&
            CreateEmptyDir(root, ROOT_NAME) != Utils::SizeTMax();
}

bool FSManager::Import(string const& root, string const& host_file, string const& fs_file) {
    size_t destdir_ptr = TryFindEntry(root, Utils::PathWithNoFileName(fs_file));
    if(destdir_ptr == Utils::SizeTMax()) {
        return false;
    }
    File file(memmanager_.block_size, host_file, Utils::GetPathTail(fs_file));
    size_t size_blocks = file.SizeInBlocks();
    if(size_blocks > memmanager_.FreeSpace()) {
        return false;
    }
    vector<size_t> blocks = memmanager_.AllocBlocks(size_blocks);
    file.Write(root, blocks);
    DirEntry de(file.Name(), blocks[0]);
    Dir destdir(memmanager_.block_size, root, destdir_ptr);
    if(!TryAddEntry(root, destdir, de)) {
        memmanager_.FreeLastAllocated();
        return false;
    }
    TraverseUpdatingSize(root, Utils::PathWithNoFileName(fs_file), file.SizeInBlocks());
    return true;
}

bool FSManager::Export(const string &root, const string &fs_file, const string &host_file) {
    size_t file_ptr = TryFindEntry(root, fs_file);
    if(file_ptr == Utils::SizeTMax() || !File::IsFile(root, file_ptr)) {
        return false;
    }
    File file(memmanager_.block_size, root, file_ptr);
    file.Export(host_file);
    return true;
}

bool FSManager::Mkdir(const string &root, const string &path) {
    if(path == ROOT_NAME || path.find(ROOT_NAME) == string::npos) {
        return false;
    }
    string pathcpy = path;
    size_t last_ptr = FindMostCommon(root, pathcpy);
    if(pathcpy.empty()) {
        return true;
    }
    size_t dirs_num = Utils::CountPathUnits(pathcpy);
    if(memmanager_.FreeSpace() < dirs_num * Dir::EMPDIR_SIZE_BLOCKS) {
        return false;
    }
    string dir_name = Utils::CutPathTail(pathcpy);
    size_t dir_ptr = CreateEmptyDir(root, dir_name);
    DirEntry de(dir_name, dir_ptr);
    for(size_t i = 1; i < dirs_num; ++i) {
        dir_name = Utils::CutPathTail(pathcpy);
        Dir dir(memmanager_.block_size, dir_name);
        dir.AddEntryExpanding(de);
        vector<size_t> blocks = memmanager_.AllocBlocks(Dir::EMPDIR_SIZE_BLOCKS);
        dir.Write(root, blocks);
        de.name = dir_name;
        de.ptr = blocks[0];
    }
    Dir last_dir(memmanager_.block_size, root, last_ptr);
    return TryAddEntry(root, last_dir, de);
}

bool FSManager::Ls(const std::string& root, const std::string& path) {
    size_t ptr = TryFindEntry(root, path);
    if(ptr == Utils::SizeTMax()) {
        return false;
    }
    if(Dir::IsDir(root, ptr)) {
        Dir dir(memmanager_.block_size, root, ptr);
        PrintDirInfo(dir);
        return true;
    } else if(File::IsFile(root, ptr)) {
        File file(memmanager_.block_size, root, ptr);
        PrintFileInfo(file);
        return true;
    }
    return false;
}

bool FSManager::Rm(const std::string &root, const std::string &path) {
    if(path == ROOT_NAME) {
        return false;
    }
    size_t ptr = TryFindEntry(root, path);
    if(ptr == Utils::SizeTMax()) {
        return false;
    }
    vector<size_t> to_delete;
    size_t freed_blocks;
    string del_entry_name;
    if(Dir::IsDir(root, ptr)) {
        Dir del_dir(memmanager_.block_size, root, ptr);
        freed_blocks = del_dir.DeleteEntries(to_delete, memmanager_.block_size, root);
        del_entry_name = del_dir.Name();
    }
    else {
        to_delete.push_back(ptr);
        File file(memmanager_.block_size, root, ptr);
        freed_blocks = file.SizeInBlocks();
        del_entry_name = file.Name();
    }
    for(size_t i = 0; i != to_delete.size(); ++i) {
        vector<size_t> blocks = BlockList<ChainableBlock, FileBodyBlock>::GetBlocksList(memmanager_.block_size, root, to_delete[i]);
        memmanager_.FreeBlocks(blocks);
    }
    size_t parent_ptr = TryFindEntry(root, Utils::PathWithNoFileName(path));
    Dir parent(memmanager_.block_size, root, parent_ptr);
    size_t deleted_block = parent.DeleteEntry(del_entry_name);
    if(deleted_block != Utils::SizeTMax()) {
        memmanager_.FreeBlock(deleted_block);
    }
    parent.Write(root);
    TraverseUpdatingSize(root, Utils::PathWithNoFileName(path), -1 * freed_blocks);
    return true;
}

bool FSManager::Copy(const std::string &root, const std::string &src, const std::string &dest) {
    if(Utils::IsParentDir(dest, src)) {
        return false;
    }
    size_t srcptr = TryFindEntry(root, src);
    size_t destptr = TryFindEntry(root, dest);
    if(srcptr == Utils::SizeTMax() ||
       destptr == Utils::SizeTMax() ||
       srcptr == destptr) {
        return false;
    }
    Dir dest_dir(memmanager_.block_size, root, destptr);
    if(dest_dir.TryFindEntry(Utils::GetPathTail(src)) != Utils::SizeTMax()) {
        return false;
    }
    vector<SearchPath> paths;
    if(Dir::IsDir(root, srcptr)) {
        Dir srcdir(memmanager_.block_size, root, srcptr);
        paths = srcdir.CollectPaths(memmanager_.block_size, root);
    }
    else {
        SearchPath sp = { Utils::GetPathTail(src), srcptr };
        paths.push_back(sp);
    }
    for(size_t i = 0; i != paths.size(); ++i) {
        if(Dir::IsDir(root, paths[i].tail_ptr)) {
            Mkdir(root, dest + '/' + paths[i].path);
        }
        else {
            string file_path_prefix = dest == ROOT_NAME ? dest : dest + '/';
            file_path_prefix += paths[i].path;
            file_path_prefix = Utils::PathWithNoFileName(file_path_prefix);
            Mkdir(root, file_path_prefix);
            size_t copied_blocks = CopyFile(root, paths[i].tail_ptr, file_path_prefix);
            if(copied_blocks == 0) {
                return false;
            }
            TraverseUpdatingSize(root, file_path_prefix, copied_blocks);
        }
    }
    return true;
}

bool FSManager::Move(const std::string &root, const std::string &src, const std::string &dest) {
    if(src == dest) {
        return true;
    }
    if(Utils::ComparePathsNesting(src, dest) < 0) {
        return false;
    }
    return Copy(root, src, dest) && Rm(root, src);
}

void FSManager::ReadConfig(string const& root, bool is_new) {
    ifstream ifs(root + '/' + CONFIG_FILE);
    string buf;
    std::getline(ifs, buf);
    size_t block_size = std::stol(buf);
    std::getline(ifs, buf);
    size_t blocks_num = std::stol(buf);
    if(is_new) {
        memmanager_.Init(block_size, blocks_num);
    }
    else {
        memmanager_.Load(block_size, root);
    }
}

size_t FSManager::TryFindEntry(string const& root, string path, size_t block) {
    if(path.empty()) {
        return block;
    }
    if(path == ROOT_NAME) {
        return std::stoi(ROOT_BLOCK);
    }
    Dir current(memmanager_.block_size, root, block);
    string head = Utils::CutPathHead(path);
    size_t ptr = current.TryFindEntry(head);
    return ptr == Utils::SizeTMax() ? Utils::SizeTMax() : TryFindEntry(root, path, ptr);
}

bool FSManager::TryAddEntry(string const& root, Dir& dir, DirEntry& de) {
    if(dir.AddEntryExpanding(de)) {
        if(memmanager_.FreeSpace() == 0) {
            return false;
        }
        dir.WriteExpanding(root, std::vector<size_t>(1, memmanager_.AllocBlock()));
    } else {
        dir.Write(root);
    }
    return true;
}

void FSManager::TraverseUpdatingSize(string const& root, string path, long additional_size, size_t block) {
    if(path == ROOT_NAME) {
        Dir root_dir(memmanager_.block_size, root, std::stoi(ROOT_BLOCK));
        root_dir.UpdateSize(additional_size);
        root_dir.Write(root);
        return;
    }
    Dir current(memmanager_.block_size, root, block);
    current.UpdateSize(additional_size);
    current.Write(root);
    if(path.empty()) {
        return;
    }
    string head = Utils::CutPathHead(path);
    size_t ptr = current.TryFindEntry(head);
    if(ptr != Utils::SizeTMax()) {
        TraverseUpdatingSize(root, path, additional_size, ptr);
    }
}

size_t FSManager::FindMostCommon(string const& root, string& path, size_t block_ptr) {
    Dir current(memmanager_.block_size, root, block_ptr);
    string head = Utils::CutPathHead(path);
    size_t nextptr = current.TryFindEntry(head);
    return nextptr == Utils::SizeTMax() ? path = head + path, block_ptr : FindMostCommon(root, path, nextptr);
}

size_t FSManager::CreateEmptyDir(string const& root, string const& name) {
    Dir empdir(memmanager_.block_size, name);
    vector<size_t> blocks = memmanager_.AllocBlocks(Dir::EMPDIR_SIZE_BLOCKS);
    if(!empdir.Write(root, blocks)) {
        return Utils::SizeTMax();
    }
    return blocks[0];
}

void FSManager::PrintDirInfo(Dir& dir) {
    std::cout << "Type: folder\n";
    std::cout << "Name: " << dir.Name() << '\n';
    std::cout << "Size: " << dir.Size() << '\n';
    std::cout << "Last modified: " << dir.LastModTime() << '\n';
    vector<string> entries = dir.EntriesList();
    std::cout << "Entires number: " << entries.size() << '\n';
    for(size_t i = 0; i != entries.size(); ++i) {
        std::cout << "- " << entries[i] << '\n';
    }
    std::cout << std::endl;
}

void FSManager::PrintFileInfo(File& file) {
    std::cout << "Type: file\n";
    std::cout << "Name: " << file.Name() << '\n';
    std::cout << "Size: " << file.SizeInBlocks() << '\n';
    std::cout << "Last modified: " << file.LastModTime() << '\n';
    std::cout << std::endl;
}

size_t FSManager::CopyFile(string const& root, size_t src_ptr, string const& destdir_path) {
    size_t destdir_ptr = TryFindEntry(root, destdir_path);
    File file_to_copy(memmanager_.block_size, root, src_ptr);
    File new_file(memmanager_.block_size, file_to_copy);
    size_t size_blocks = new_file.SizeInBlocks();
    if(size_blocks > memmanager_.FreeSpace()) {
        return 0;
    }
    vector<size_t> blocks = memmanager_.AllocBlocks(size_blocks);
    new_file.Write(root, blocks);
    DirEntry de(new_file.Name(), blocks[0]);
    Dir destdir(memmanager_.block_size, root, destdir_ptr);
    if(!TryAddEntry(root, destdir, de)) {
        memmanager_.FreeLastAllocated();
        return 0;
    }
    return new_file.SizeInBlocks();
}

//string FSManager::FullPath(string const& root, DirEntry de, size_t current_block) {
//    Dir current(memmanager_.block_size, root, current_block);
//    if(current_block == de.ptr) {
//        return current.Name();
//    }
//    size_t ptr = current.TryFindEntry(de.name);
//    string res = FullPath(root, de, ptr);
//    if(current_block == std::stoul(ROOT_BLOCK)) {
//        return current.Name() + res;
//    }
//    return current.Name() + '/' + res;
//}
