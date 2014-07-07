
#ifndef LIST_H
#define LIST_H

#include "myfs.h"

struct ListOnBlocksBlock {
    static const size_t header_size = sizeof(block_id_t);

    ListOnBlocksBlock(BlockPtr block, size_t size_of_slot) 
        : block_(move(block)), size_of_slot_(size_of_slot)
    {
       slots_count_ = (MyFileSystem::instance().block_size() - sizeof(block_id_t)) / size_of_slot_; 
    }
    
    void load() {
        next_block_ = block_->get_int<block_id_t>(0);
    }
    
    void set_next(block_id_t id) {
        block_->put_int(0, id);
        next_block_ = id;
    }

    block_id_t next_block() {
        return next_block_;
    }

    void free_slot(size_t slot_index) {
        block_->put_int<block_id_t>(slot_offset(slot_index), 0);
    }

    size_t slot_offset(size_t slot_index) {
        return slot_index * size_of_slot_ + header_size;
    }

    size_t slots_count() {
        return slots_count_;
    }
    
    BlockPtr block_;
    size_t size_of_slot_;
    size_t slots_count_;
    block_id_t next_block_;
};

#endif
