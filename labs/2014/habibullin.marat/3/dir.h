#ifndef DIR_H
#define DIR_H

#include "blocklist.h"
#include "dirheadblock.h"
#include "direntriesblock.h"
#include "direntry.h"

struct SearchPath {
    std::string path;
    size_t tail_ptr;
};

class Dir {
public:
    Dir(size_t block_size, string const& name);
    Dir(size_t block_size, std::string const& root, size_t beg_ptr) : blocks_(block_size) {
        blocks_.Read(root, beg_ptr);
    }

    size_t RawSizeInBlocks() { return blocks_.Size(); }
    bool Write(string const& root, std::vector<size_t> block_ptrs) { return blocks_.Write(root, block_ptrs); }
    bool Write(string const& root) { return blocks_.Write(root); }
    bool WriteExpanding(std::string const& root, std::vector<size_t> const& additional_blocks) {
        return blocks_.WriteExpanding(root, additional_blocks);
    }

    size_t TryFindEntry(std::string const& name);
    bool AddEntryExpanding(const DirEntry &de);

    void UpdateSize(long additional) { blocks_.head.SetSize(blocks_.head.GetSize() + additional); }

    size_t DeleteEntries(std::vector<size_t>& to_delete, size_t block_size, string const& root);
    size_t DeleteEntry(string const& name);

    vector<SearchPath> CollectPaths(size_t block_size, string const& root);

    static bool IsDir(std::string const& root, size_t block_ptr) { return DirHeadBlock::IsDirHead(root, block_ptr); }
    std::string Name() { return blocks_.head.GetName(); }
    size_t Size() { return blocks_.head.GetSize(); }
    std::string LastModTime() { return blocks_.head.GetLastModTime(); }
    std::vector<std::string> EntriesList();

    static const size_t EMPDIR_SIZE_BLOCKS = 2;
private:
    BlockList<DirHeadBlock, DirEntriesBlock> blocks_;
};

#endif // DIR_H
