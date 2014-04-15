#include "memoryallocator.h"

int MemoryAllocator::allocate(Block* root, size_t bytes) {
    if(!root) {
        return -1;
    }

    if(root->free && bytes <= root->data_size()) {
        root->free = false;
        if(root->data_ptr() + bytes + sizeof(Block) <
                (root->next ? root->next->ptr : n)) {
            Block* data_block = new Block(n);
            data_block->ptr = root->ptr + sizeof(Block) + bytes;
            data_block->next = root->next;
            root->next = data_block;
        }
        return root->data_ptr();
    } else {
        return allocate(root->next, bytes);
    }
}


void MemoryAllocator::merge_blocks(Block* block) {
    Block* next = block->next;
    block->next = next->next;
    delete next;
}

int MemoryAllocator::free_block(Block* cur, size_t data_ptr) {
    if(!cur) {
        return -1;
    }

    if(cur->data_ptr() == data_ptr) {
        cur->free = true;
        if(cur->next && cur->next->free) {
            merge_blocks(cur);
        }
        return 0;
    } else {
        int result = free_block(cur->next, data_ptr);
        if(!result && cur->free && cur->next && cur->next->free) {
            merge_blocks(cur);
        }
        return result;
    }
}

void MemoryAllocator::deleteAll(Block* block) {
    if(!block->next) {
        return;

    }
    deleteAll(block->next);
    delete block;
}

void MemoryAllocator::get_info(Block& block, size_t& blocks_count,
              size_t& total_memory_alloc, size_t& max_block_size) {

    Block* cur = &block;
    while (true) {
        if(!cur->free) {
            ++blocks_count;
            total_memory_alloc += cur->data_size();
        } else if(max_block_size < cur->data_size()) {
            max_block_size = cur->data_size();
        }
        if (cur->next) {
            cur = cur->next;
        } else {
            break;
        }
    }
}

void MemoryAllocator::print_map(Block &block) {
    Block* cur = &block;
    while(true) {

        for(size_t i = 0; i < cur->data_size(); ++i) {
            std::cout<< 'm';
        }
        for(size_t i = 0; i < cur->data_size(); ++i) {
            std::cout << (cur->free ? 'f' : 'u');
        }
        if (cur->next) {
            cur = cur->next;
        } else {
            break;
        }
    }
}
