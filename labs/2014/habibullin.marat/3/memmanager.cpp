#include "memmanager.h"

#include "blocklist.h"
#include "bitsetheadblock.h"
#include "bitsetbodyblock.h"

using std::vector;

bool MemManager::Init(size_t bs, size_t bn) {
    if(bn < RESERVED_BLOCKS_NUM) {
        return false;
    }
    block_size = bs;
    blocks_bitset_.Resize(bn);
    for(size_t i = 0; i != RESERVED_BLOCKS_NUM; ++i) {
        blocks_bitset_.Set(i + FIRST_RESERVED);
        reserved_blocks_.push_back(i + FIRST_RESERVED);
    }
    return true;
}

void MemManager::Dump(const string &root) {
    BlockList<BitSetHeadBlock, BitSetBodyBlock> bslist(block_size);
    bslist.head.SetSize(blocks_bitset_.size());
    bslist.head.SetNumOfSetBits(blocks_bitset_.size() - blocks_bitset_.UnsetBits());

    vector<char> bytes = blocks_bitset_.Bytes();
    for(size_t i = 0; i != bytes.size(); ) {
        BitSetBodyBlock bsbb(block_size);
        size_t max_cap = bsbb.MaxCapacity();
        if(bytes.size() - i <= max_cap) {
            vector<char> part(bytes.begin() + i, bytes.end());
            bsbb.SetData(part.data(), part.size());
            bslist.Add(bsbb);
            break;
        }
        vector<char> part;
        for(size_t j = 0; j != max_cap; ++j) {
            part.push_back(bytes[i++]);
        }
        bsbb.SetData(part.data(), part.size());
        bslist.Add(bsbb);
    }
    while(bslist.Size() != RESERVED_BLOCKS_NUM) {
        bslist.Add(BitSetBodyBlock(block_size));
    }
    bslist.Write(root, reserved_blocks_);
}

void MemManager::Load(size_t bs, const string &root) {
    block_size = bs;
    BlockList<BitSetHeadBlock, BitSetBodyBlock> bslist(block_size);
    bslist.Read(root, FIRST_RESERVED);

    blocks_bitset_.Init(bslist.head.GetSize(), bslist.head.GetNumOfSetBits());
    typename BlockList<BitSetHeadBlock, BitSetBodyBlock>::Iterator iter = bslist.ElemBegIterator();
    typename BlockList<BitSetHeadBlock, BitSetBodyBlock>::Iterator end = bslist.EndIterator();
    for(; iter != end; ++iter) {
        size_t max_cap = (*iter).MaxCapacity();
        char* buffer = new char[block_size];
        size_t size_bytes = (*iter).GetData(buffer);
        blocks_bitset_.Bytes().insert(blocks_bitset_.Bytes().end(), buffer, buffer + size_bytes);
        delete[] buffer;
        if(size_bytes != max_cap) {
            break;
        }
    }
    for(size_t i = 0; i != RESERVED_BLOCKS_NUM; ++i) {
        reserved_blocks_.push_back(i + FIRST_RESERVED);
    }
}

vector<size_t> MemManager::AllocBlocks(size_t num) {
    vector<size_t> blocks;
    last_allocated_.resize(0);
    for(size_t i = 0; i != num; ++i) {
        size_t block = Alloc();
        blocks.push_back(block);
        last_allocated_.push_back(block);
    }
    return blocks;
}

size_t MemManager::AllocBlock() {
    size_t block = Alloc();
    last_allocated_.resize(0);
    last_allocated_.push_back(block);
    return block;
}

void MemManager::FreeLastAllocated() {
    for(size_t i = 0; i != last_allocated_.size(); ++i) {
        blocks_bitset_.Unset(last_allocated_.at(i));
    }
    last_allocated_.resize(0);
}

void MemManager::FreeBlocks(vector<size_t> blocks) {
    for(size_t i = 0; i != blocks.size(); ++i) {
        blocks_bitset_.Unset(blocks[i]);
    }
}

size_t MemManager::Alloc() {
    size_t unset = blocks_bitset_.FirstUnset();
    blocks_bitset_.Set(unset);
    return unset;
}
