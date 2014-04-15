#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <cstdlib>
#include <iostream>
#include <string>

struct MemControlBlock {
    bool avaible;
    MemControlBlock* prev;
    MemControlBlock* next;

    MemControlBlock()
        : avaible(true)
        , prev(0)
        , next(0)
    {}
    MemControlBlock(MemControlBlock* p, MemControlBlock* n)
        : avaible(true)
        , prev(p)
        , next(n)
    {
        if (n != 0) {
            n->prev = this;
        }
        if (p != 0) {
            p->next = this;
        }
    }

};

class Allocator
{
public:
    Allocator();
    ~Allocator();
    void init(size_t N);
    size_t alloc(size_t s);
    size_t free(size_t p);
    void info();
    void map();
private:
    void tryToMergeBlocks(MemControlBlock* memBlock);
    void deleteBlock(MemControlBlock* memBlock);
    size_t blockDataSize(MemControlBlock* memBlock);
    size_t makeAllocation(MemControlBlock* memBlock, size_t size);
    size_t m_buf_size;
    char* m_buf;


};

#endif // ALLOCATOR_H
