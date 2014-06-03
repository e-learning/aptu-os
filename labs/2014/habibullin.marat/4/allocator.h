#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <vector>
#include "headerprocessor.h"

using std::vector;

class Allocator {
public:
    Allocator(int size);
    void Alloc(int size);
    void Free(unsigned int ptr);
    void Map();
    void Info();
private:
    void CreateBlock(int from, int to, bool is_free);
    int FindFirstFit(unsigned int size);
    BlockHeader GetBlockHeader(int start);
    int NextBlockHeaderStart(int cur_bh_start, BlockHeader& bh);
    bool TryMergeTwoBlocks(int start1, int start2);
    int MaxFreeBlockSize();
    bool IsBH(int ptr);

    vector<char> mem_;
    int blocks_counter_;
    int allocated_mem_counter_;
};

#endif // ALLOCATOR_H
