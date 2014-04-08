#ifndef FSMANAGER_H
#define FSMANAGER_H

#include <string>
#include "memmanager.h"
#include "dir.h"
#include "file.h"

class FSManager {
public:
    FSManager(string const& root, bool is_new) { ReadConfig(root, is_new); }

    void Init(string const& root);
    bool Format(string const& root);
    bool Import(string const& root, string const& host_file, string const& fs_file);
    bool Export(string const& root, string const& fs_file, string const& host_file);
    bool Mkdir(string const& root, string path);
    bool Ls(string const& root, string path);
    bool Rm(string const& root, string path);
    bool Copy(string const& root, string src, string dest);
    bool Move(string const& root, string const& src, string const& dest);

    void Close(string const& root) { memmanager_.Dump(root); }
private:
    void ReadConfig(string const& config_path, bool is_new);
    size_t TryFindEntry(string const& root, string path, size_t block = 0);
    bool TryAddEntry(string const& root, Dir& dir, DirEntry& de);
    void TraverseUpdatingSize(string const& root, string path, long additional_size, size_t block = 0);
    size_t FindMostCommon(string const& root, string& path, size_t block_ptr = 0);
    size_t CreateEmptyDir(string const& root, string const& name);
    void PrintDirInfo(Dir& dir);
    void PrintFileInfo(File& file);
    size_t CopyFile(string const& root, size_t src_ptr, string const& destdir_path);

    MemManager memmanager_;

    const string CONFIG_FILE = "config";
    const string ROOT_BLOCK = "0";
    const string ROOT_NAME = "/";
};

#endif // FSMANAGER_H
