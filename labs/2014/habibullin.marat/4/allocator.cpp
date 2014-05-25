#include "allocator.h"
#include "headerprocessor.h"

#include <iostream>

Allocator::Allocator(int size) :
    mem_(size),
    blocks_counter_(0),
    allocated_mem_counter_(0)
{
    CreateBlock(0, size, true);
}

void Allocator::Alloc(int size) {
    int free_space = mem_.size() - allocated_mem_counter_ - 2 * HEADER_SIZE_BYTES * blocks_counter_;
    if(free_space < size) {
        std::cout << '-' << std::endl;
        return;
    }
    int bh_start = FindFirstFit(size);
    if(bh_start < 0) {
        std::cout << '-' << std::endl;
        return;
    }
    BlockHeader old_bh = GetBlockHeader(bh_start);
    if(old_bh.size - size < 1 + 2 * (int)HEADER_SIZE_BYTES) {
        size = old_bh.size;
    }
    CreateBlock(bh_start, bh_start + size + 2 * HEADER_SIZE_BYTES, false);
    ++blocks_counter_;
    allocated_mem_counter_ += size;
    std::cout << "+ " << bh_start + HEADER_SIZE_BYTES << std::endl;
    unsigned int after_new_block_ptr = bh_start + size + 2 * HEADER_SIZE_BYTES;
    if(after_new_block_ptr < mem_.size() && (old_bh.size - size) != 0) {
        CreateBlock(after_new_block_ptr, bh_start + old_bh.size + 2 * HEADER_SIZE_BYTES, true);
    }
}

void Allocator::Free(unsigned int ptr) {
    if(!IsBH(ptr - HEADER_SIZE_BYTES)) {
        std::cout << "-" << std::endl;
        return;
    }
    BlockHeader bhc = GetBlockHeader(ptr - HEADER_SIZE_BYTES);
    if(bhc.is_free) {
        std::cout << "-" << std::endl;
        return;
    }
    CreateBlock(ptr - HEADER_SIZE_BYTES, ptr + bhc.size + HEADER_SIZE_BYTES, true);
    if(ptr + bhc.size + HEADER_SIZE_BYTES < mem_.size()) {
        TryMergeTwoBlocks(ptr - HEADER_SIZE_BYTES, ptr + bhc.size + HEADER_SIZE_BYTES);
    }
    if((int)ptr - (int)HEADER_SIZE_BYTES * 2 >= 0) {
        BlockHeader bhl = GetBlockHeader(ptr - 2 * HEADER_SIZE_BYTES);
        TryMergeTwoBlocks(ptr - bhl.size - 3 * HEADER_SIZE_BYTES, ptr - HEADER_SIZE_BYTES);
    }
    std::cout << "+" << std::endl;
    --blocks_counter_;
    allocated_mem_counter_ -= bhc.size;
}

void Allocator::Map() {
//    for (size_t i = 0; i != mem_.size(); ++i) {
//        std::cout << i << ' ';
//    }
    std::cout << std::endl;
    int i = 0;
    BlockHeader bh;
    do {
        bh = GetBlockHeader(i);
        std::cout << "m m ";
        if(bh.is_free) {
            for(int j = 0; j != bh.size; ++j) {
                std::cout << "f ";
            }
        }
        else {
            for(int j = 0; j != bh.size; ++j) {
                std::cout << "u ";
            }
        }
        std::cout << "m m ";
    }
    while ((i = NextBlockHeaderStart(i, bh)) > 0);
    std::cout << std::endl;
}

void Allocator::Info() {
    std::cout << blocks_counter_  << ' ' << blocks_counter_ << ' ';
    int max_free = MaxFreeBlockSize();
    std::cout << max_free << std::endl;
}

void Allocator::CreateBlock(int from, int to, bool is_free) {
    int size = (to - from) - 2 * HEADER_SIZE_BYTES;
    RawHeader rh = HeaderProcessor::createHeader(BlockHeader { size, is_free });
    int j = 0;
    for(size_t i = from; i != from + HEADER_SIZE_BYTES; ++i) {
        mem_[i] = rh[j++];
    }
    j = 0;
    for(int i = to - HEADER_SIZE_BYTES; i != to; ++i) {
        mem_[i] = rh[j++];
    }
}

int Allocator::FindFirstFit(unsigned int size) {
    int i = 0;
    BlockHeader bh;
    do {
        bh = GetBlockHeader(i);
        if(bh.is_free && (unsigned int)bh.size >= size) {
            return i;
        }
    }
    while ((i = NextBlockHeaderStart(i, bh)) > 0);
    return -1;
}

BlockHeader Allocator::GetBlockHeader(int start) {
    RawHeader rh;
    rh[0] = mem_[start];
    rh[1] = mem_[start + 1];
    return HeaderProcessor::readHeader(rh);
}

int Allocator::NextBlockHeaderStart(int cur_bh_start, BlockHeader &bh) {
    unsigned int index = cur_bh_start + bh.size + HEADER_SIZE_BYTES * 2;
    return index < mem_.size() ? index : -1;
}

bool Allocator::TryMergeTwoBlocks(int start1, int start2) {
    BlockHeader bh1 = GetBlockHeader(start1);
    BlockHeader bh2 = GetBlockHeader(start2);
    if(!bh1.is_free || !bh2.is_free) {
        return false;
    }
    CreateBlock(start1, start2 + bh2.size + 2 * HEADER_SIZE_BYTES, true);
    return true;
}

int Allocator::MaxFreeBlockSize() {
    int max_size = 0;
    int i = 0;
    BlockHeader bh;
    do {
        bh = GetBlockHeader(i);
        max_size = (bh.is_free && max_size < bh.size) ? bh.size : max_size;
    }
    while ((i = NextBlockHeaderStart(i, bh)) > 0);
    return max_size;
}

bool Allocator::IsBH(int ptr) {
    int i = 0;
    BlockHeader bh;
    do {
        if(i == ptr) {
            return true;
        }
        bh = GetBlockHeader(i);
    }
    while ((i = NextBlockHeaderStart(i, bh)) > 0);
    return false;
}
