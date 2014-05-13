#ifndef FILE_H
#define FILE_H

#include "blocklist.h"
#include "fileheadblock.h"
#include "filebodyblock.h"
#include "direntry.h"

using std::string;

class File {
public:
    File(size_t block_size, string const& ext_path, string const& name);
    File(size_t block_size, string const& root, size_t beg_ptr) : blocks_(block_size) {
        blocks_.Read(root, beg_ptr);
    }
    File(size_t block_size, File& other, string const& new_file_name);

    size_t SizeInBlocks() { return blocks_.Size(); }
    bool Write(string const& root, std::vector<size_t> block_ptrs) {
        return blocks_.Write(root, block_ptrs);
    }
    void Export(string const& path);
    std::string Name() { return blocks_.head.GetName(); }
    std::string LastModTime() { return blocks_.head.GetLastModTime(); }

    static bool IsFile(std::string const& root, size_t block_ptr) { return FileHeadBlock::IsFileHead(root, block_ptr); }
private:
    BlockList<FileHeadBlock, FileBodyBlock> blocks_;
};

#endif // FILE_H
