#include "Allocator.h"
#include <iostream>


Allocator::Allocator(size_t n){
    memory_allocated = n + sizeof(MemoryBlockDiscriptor);
    memory = new char[memory_allocated];
    blks_list = new MemoryBlockDiscriptor;
    blks_list->used = 0;
    blks_list->size = n;
    blks_list->data_pointer = sizeof(MemoryBlockDiscriptor);
    blks_list->next = NULL;
    blks_list->free = true;
}

Allocator::~Allocator(){
    delete[] memory;
}

int Allocator::alloc(int size)
{
    MemoryBlockDiscriptor *cur_blk;
    MemoryBlockDiscriptor *new_blk;
    
    cur_blk = blks_list;
    
    while(cur_blk) {
        if(cur_blk->free) {
            if(cur_blk->size >= size) {
                cur_blk->free = false;
                cur_blk->used = size;
                if(cur_blk->data_pointer + size + sizeof(MemoryBlockDiscriptor) < 
                        ((cur_blk->next) ? cur_blk->next->data_pointer : memory_allocated)) {
                    new_blk = new MemoryBlockDiscriptor;
                    new_blk->data_pointer = cur_blk->data_pointer + size + sizeof(MemoryBlockDiscriptor);
                    new_blk->free = true;
                    new_blk->size = cur_blk->size - size;
                    new_blk->used = 0;
                    new_blk->next = cur_blk->next;
                    cur_blk->next = new_blk;
                    cur_blk->size = size;
                }
                return cur_blk->data_pointer;
            }
        }
        
        cur_blk = cur_blk->next;
    }
    
    return -1;
}

void Allocator::info() {
    MemoryBlockDiscriptor *cur_blk;
    int blocks_for_user;
    size_t total_allocated;
    size_t max_allocatable;
    
    cur_blk = blks_list;
    max_allocatable = 0;
    total_allocated = 0;
    blocks_for_user = 0;
    
    while(cur_blk) {
        if(!cur_blk->free) {
            blocks_for_user++;
            total_allocated += cur_blk->used;
        }
        else {
            if(cur_blk->size > max_allocatable)
                max_allocatable = cur_blk->size;
        }
        
        cur_blk = cur_blk->next;
    }
    
    std::cout << "Info:" << std::endl;
    
    std::cout << "Блоков выделено пользователю = " << blocks_for_user << std::endl;
    std::cout << "Объем памяти выделенный пользователю = " << total_allocated << std::endl;
    std::cout << "Максимально можно выделить = " << max_allocatable << std::endl;
}

void Allocator::map() {
    int i;
    MemoryBlockDiscriptor *cur_blk;
    
    cur_blk = blks_list;
    
    std::cout << "Memory map:" << std::endl;
    
    while(cur_blk) {
        for(i = 0; i < sizeof(MemoryBlockDiscriptor) ; i++)
            std::cout << "m";
        for(i = 0; i < cur_blk->used ; i++)
            std::cout << "u";
        for(i = 0; i < cur_blk->size - cur_blk->used ; i++)
            std::cout << "f";
        
        cur_blk = cur_blk->next;
    }
    
    std::cout << std::endl;
}

int Allocator::free(size_t ptr) {
    MemoryBlockDiscriptor *cur_blk;
    
    cur_blk = blks_list;
    
    while(cur_blk) {
        if(cur_blk->data_pointer == ptr) {
            cur_blk->free = true;
            cur_blk->used = 0;
            join_free_blocks();
            return 0;
        }
        cur_blk = cur_blk->next;
    }
    
    return -1;
}

void Allocator::join_free_blocks() {
    MemoryBlockDiscriptor *cur_blk;
    
    cur_blk = blks_list;
    
    while(cur_blk) {
        if(cur_blk->free) {
            if(cur_blk->next && cur_blk->next->free) {
                cur_blk->size += cur_blk->next->size;
                cur_blk->next = cur_blk->next->next;
            }
        }
        
        cur_blk = cur_blk->next;
    }
}
