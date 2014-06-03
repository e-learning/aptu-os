#include "allocator.h"
#include <new>
#include <sstream>

Allocator::Allocator()
{
}

Allocator::~Allocator()
{
    delete m_buf;
}

size_t Allocator::free(size_t p)
{
    size_t res = 1;
    MemControlBlock* block = reinterpret_cast<MemControlBlock*> (m_buf);
    MemControlBlock* deletedBlock = reinterpret_cast<MemControlBlock*> (m_buf) + p - sizeof(MemControlBlock);
    bool isFind = false;
    while (block != 0) {
        if (block == deletedBlock) {
            isFind = true;
            break;
        }
        block = block->next;
    }
    if (isFind && !deletedBlock->avaible) {
        deleteBlock(deletedBlock);
        res = 0;
    }
    return res;
}

void Allocator::info()
{
    MemControlBlock* memBlock = reinterpret_cast<MemControlBlock*>(m_buf);
    size_t blocksCount = 0;
    size_t allocSize = 0;
    size_t maxAvaible = 0;
    while (memBlock != 0) {
        size_t size = blockDataSize (memBlock);
        if(!memBlock->avaible) {
            blocksCount++;
            allocSize += size;
        } else {

            maxAvaible = std::max(maxAvaible, size);
        }
        memBlock = memBlock->next;
    }

    std::cout << blocksCount << " " << allocSize << " " << maxAvaible << std::endl;
}

void Allocator::map()
{
    std::string metaInfo(sizeof(MemControlBlock), 'm');
    MemControlBlock* memBlock = reinterpret_cast<MemControlBlock*> (m_buf);
    char c;
    while (memBlock != 0) {
        std::cout << metaInfo;
        size_t size = blockDataSize (memBlock);
        bool flag = memBlock->avaible;
        memBlock = memBlock->next;
	if (flag) {
            c = 'f';
        } else {
            c = 'u';
        }
        for (size_t i = 0; i < size; i++) {
            std::cout << c;
        }
    }
    std::cout << std::endl;

}

void Allocator::tryToMergeBlocks(MemControlBlock *memBlock)
{
    if (memBlock->prev != 0 && memBlock->prev->avaible) {
        if (memBlock->next != 0 && memBlock->next->avaible) {
            memBlock->prev->next = memBlock->next->next;
            if (memBlock->prev->next != 0) {
                memBlock->prev->next->prev = memBlock->prev;
            }
        } else {
            memBlock->prev->next = memBlock->next;
            if (memBlock->next != 0)
                memBlock->next->prev = memBlock->prev;
        }
    } else if (memBlock->next != 0 && memBlock->next->avaible) {
        memBlock->next = memBlock->next->next;
        if (memBlock->next != 0)
            memBlock->next->prev = memBlock;
    }

}

void Allocator::deleteBlock(MemControlBlock *memBlock)
{
    memBlock->avaible = true;
    tryToMergeBlocks(memBlock);
}

size_t Allocator::blockDataSize(MemControlBlock *memBlock)
{
    if (memBlock->next == 0) {
        return (reinterpret_cast<MemControlBlock*> (m_buf) + m_buf_size - memBlock - sizeof(MemControlBlock));
    } else {
        return (memBlock->next - memBlock - sizeof(MemControlBlock));
    }

}

size_t Allocator::makeAllocation(MemControlBlock *memBlock, size_t size)
{
    if (blockDataSize (memBlock) > (size + sizeof(MemControlBlock) + 1)) {
        MemControlBlock* newBlock = memBlock + sizeof(MemControlBlock) + size;
        new (newBlock) MemControlBlock(memBlock, memBlock->next);
    }
    memBlock->avaible = false;
    return memBlock + sizeof(MemControlBlock) - reinterpret_cast<MemControlBlock*> (m_buf);
}

void Allocator::init(size_t N)
{
    m_buf_size = N;
    m_buf = new char[m_buf_size];
    new (m_buf) MemControlBlock();
}

size_t Allocator::alloc(size_t s)
{
    int res = 1;
    MemControlBlock* memBlock = reinterpret_cast<MemControlBlock*>(m_buf);
    while (memBlock != 0) {
        if (memBlock->avaible && blockDataSize (memBlock) >= s) {
            res = makeAllocation(memBlock, s);
            break;
        }
        memBlock = memBlock->next;
    }
    return res;
}


