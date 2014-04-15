#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include <iostream>

struct Block {
    size_t ptr;
    Block* next;
    bool free;
    size_t n;

    explicit Block(size_t n0):
        ptr(0), next(0),free(true), n(n0) {}

    size_t data_size() {
        if(next) {
            return next->ptr - data_ptr();
        } else {
            return n - data_ptr();
        }
    }

    size_t data_ptr() {
        return ptr + sizeof(Block);
    }
};


class MemoryAllocator
{
public:

    explicit MemoryAllocator(size_t n0) : n(n0) {}

    int allocate(Block* root, size_t bytes);

    void merge_blocks(Block* block);

    int free_block(Block* cur, size_t data_ptr);

    void deleteAll(Block* block);

    void get_info(Block& block, size_t& blocks_count,
                  size_t& total_memory_alloc, size_t& max_block_size);

    void print_map(Block& block);



private:
        size_t n;
};

#endif // MEMORYALLOCATOR_H
