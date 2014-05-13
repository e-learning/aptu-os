#ifndef MEMMANAGER_H
#define MEMMANAGER_H

#include <string>
#include <vector>

#include "bitset.h"

using std::string;
using std::vector;

class MemManager {
public:
    MemManager() {}

    bool Init(size_t bs, size_t bn);
    void Dump(string const& root);
    void Load(size_t bs, string const& root);

    size_t BlocksNum() { return blocks_bitset_.size(); }
    vector<size_t> AllocBlocks(size_t num);
    size_t AllocBlock();
    void FreeLastAllocated();
    size_t FreeSpace() { return blocks_bitset_.UnsetBits(); }
    void FreeBlocks(vector<size_t> blocks);
    void FreeBlock(size_t block) { blocks_bitset_.Unset(block); }

    size_t block_size;
private:
    size_t Alloc();

    BitSet blocks_bitset_;
    vector<size_t> last_allocated_;
    const size_t RESERVED_BLOCKS_NUM = 8;
    vector<size_t> reserved_blocks_;
    const size_t FIRST_RESERVED = 2;
};
#endif // MEMMANAGER_H
