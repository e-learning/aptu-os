
#ifndef BLOCK_LIST_H
#define BLOCK_LIST_H

#include <iterator>
#include <iostream>
#include "myfs.h"
#include "list.h"

struct BlockListBlock : ListOnBlocksBlock {

    BlockListBlock(BlockPtr block) 
        : ListOnBlocksBlock(move(block), sizeof(block_id_t))
    {
        load();
    }

    void load() {
        ListOnBlocksBlock::load();
        
        free_from_slot_ = slots_count();
        blocks_.clear();
        
        for (size_t slot = 0; slot < slots_count(); slot++) {
            block_id_t id = block_->get_int<block_id_t>(slot_offset(slot));
            
            if (id == 0) {
                free_from_slot_ = slot;
                break;
            }

            blocks_.push_back(id);
        }
    }

    bool full() {
        return free_from_slot_ == slots_count();
    }

    bool empty() {
        return free_from_slot_ == 0;
    }

    void add(block_id_t id) {
        blocks_.push_back(id);
        block_->put_int(slot_offset(free_from_slot_++), id);
    }

    block_id_t pop() {
        block_id_t result = blocks_.back();
        blocks_.pop_back();
        
        block_->put_int(slot_offset(--free_from_slot_), 0);
        
        return result;
    }

    void clear() {
        block_->clear_data();
        blocks_.clear();
        free_from_slot_ = 0;
    }

    vector<block_id_t> blocks_;
    size_t free_from_slot_;
};

typedef shared_ptr<BlockListBlock> BlockListBlockPtr;
struct BlockList {
    BlockList(block_id_t first_block)
         : first_block_(first_block)
    {
    
    }

    struct Iter : iterator<forward_iterator_tag, block_id_t> {
        explicit Iter() {
            current_block_ = nullptr;
        }

        explicit Iter(block_id_t first_block_id) 
        {
            current_block_ = load_block(first_block_id);
            current_ = current_block_->blocks_.begin();            
            move_until_next();
        }

        BlockListBlockPtr current_block_;
        vector<block_id_t>::iterator current_;

        Iter & operator++() {
            ++current_;
            move_until_next();
            return *this;
        }

        void move_until_next() {
            if ((current_) == current_block_->blocks_.end()) {
                size_t next_block_id = current_block_->next_block(); 
                
                if (next_block_id == 0) {
                    current_block_ = nullptr;
                } else {
                    current_block_ = load_block(next_block_id);
                    current_ = current_block_->blocks_.begin();
                }
            }
        }

        block_id_t const & operator*() {
            return *current_;
        }

        bool operator==(const Iter & other) {
            if (current_block_ == nullptr || other.current_block_ == nullptr) {
                return current_block_ == other.current_block_;
            }

            return current_block_ == other.current_block_ && current_ == other.current_;
        }

        bool operator!=(const Iter & other) {
            return ! (*this == other);
        }
    };

    typedef block_id_t value_type;
    typedef Iter iterator;
    
    Iter begin() {
        return Iter(first_block_);
    }

    Iter end() {
        return Iter();
    }

    void init() {
        load_block(first_block_)->block_->clear_data();
    }

    void insert_after(Iter & iter, block_id_t id) {
        auto block = iter.current_block_;
        
        if (!block) {
            block = load_block(first_block_);
        }

        if (block->full()) {
            block_id_t free_block_id = MyFileSystem::instance().acquire_free_block();
            block->set_next(free_block_id);
            write_block(block);

            block = load_block(free_block_id);
            block->block_->clear_data();
            block->load();
        }
        
        block->add(id);
        write_block(block);
        iter.current_block_ = block;
    }
    
    void add(block_id_t id) {
        MyFileSystem & fs = MyFileSystem::instance();
        
        auto first_block = load_block(first_block_);
        
        if (first_block->full()) {
            block_id_t free_block_id = is_free_list() ? id : fs.acquire_free_block();
            BlockPtr new_block = fs.load_block(free_block_id);

            new_block->clear_data();

            first_block->block_->swap_data(*new_block);
            fs.write_block(*new_block);
            
            first_block->load();
            first_block->set_next(free_block_id);
            
            if (!is_free_list()) {
                first_block->add(id);
            }

        } else {
            first_block->add(id);
        }

        write_block(first_block);
    }
    
    block_id_t pop() {
        MyFileSystem & fs = MyFileSystem::instance();
        
        auto first_block = load_block(first_block_);
        
        block_id_t result = 0;

        if (first_block->empty()) {
            block_id_t second_block_id = first_block->next_block();
            auto second_block = load_block(second_block_id);
            first_block->block_->swap_data(*(second_block->block_));
            first_block->load();

            result = is_free_list() ? second_block_id : first_block->pop();
            
            if (!is_free_list()) {
                fs.release_block(second_block_id);
            }

        } else {
            result = first_block->pop();
        }

        write_block(first_block);

        return result;
    }
    
    bool empty() {
        return begin() == end();
    }
    
    void release_all_blocks() {
        MyFileSystem & fs =  MyFileSystem::instance();
        for_each(begin(), end(), [&fs](block_id_t id){
            fs.release_block(id);
        });

        block_id_t next = load_block(first_block_)->next_block();

        while (next != 0) {
            block_id_t next_next = load_block(next)->next_block();
            fs.release_block(next);
            next = next_next;
        }
    }

    bool is_free_list() {
        return first_block_ == free_blocks_first; 
    }

    static BlockListBlockPtr load_block(block_id_t id) {
        return BlockListBlockPtr(new BlockListBlock(MyFileSystem::instance().load_block(id))); 
    }

    static void write_block(const BlockListBlockPtr & block) {
        MyFileSystem::instance().write_block(*(block->block_));
    }
           
    block_id_t first_block_;
};

#endif
