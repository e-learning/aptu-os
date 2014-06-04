#include "memoryStorage.h"
#include <string>

MemoryStorage::MemoryStorage(int size) : mySize(size), firstBlock(new Block(mySize - HEADER_SIZE)) {}

MemoryStorage::~MemoryStorage()
{
    Block* current = firstBlock;
    while(current != NULL)
    {
        Block* ptr = current;
        current = current->next();
        delete ptr;
    }
}

int MemoryStorage::allocate(int size)
{
    if (size <= 0)
        return -1;
    Block* current = firstBlock;
    int offset = 0;

    while (current != NULL)
    {
        if (current->isFree() && current->size() >= size)
        {
            if (current->size() - size >= MIN_BLOCK_SIZE)
            {
                Block* newBlock = new Block(current->size() - size - HEADER_SIZE);
                current->setSize(size);
                newBlock->setPrevBlock(current);
                newBlock->setNextBlock(current->next());
                if (current->next() != NULL)
                    current->next()->setPrevBlock(newBlock);
                current->setNextBlock(newBlock);
            }
            current->allocate();
            return offset + HEADER_SIZE;
        }
        
        offset += HEADER_SIZE + current->getRealSize();
        current = current->next();
    }

    return -1;
}

int MemoryStorage::free(int offset)
{
    if (offset < HEADER_SIZE)
        return -1;
    
    int current_offset = 0;
    Block* current = firstBlock;
    while (true) 
    {
        if (!current->isFree() && current_offset + HEADER_SIZE == offset)
        {
            current->free();
            current->mergeWithNext();
            current->mergeWithPrev();
            return 0;
        }
        if (current->next() == NULL)
            return -1;
        current_offset += HEADER_SIZE + current->getRealSize();
        current = current->next();          
    }
}

MemInfo MemoryStorage::getInfo()
{
    MemInfo inf;
    Block* current = firstBlock;
    while (current != NULL)
    {
        if (!current->isFree())
        {
            ++inf.allocBlocks;
            inf.allocMemory += current->getRealSize();
        } else {
            inf.maxAlloc = std::max(inf.maxAlloc, current->size());
        }
        current = current->next();
    }
    return inf;
}

std::string MemoryStorage::toString()
{
    std::string result(mySize, 'f');

    int sPos = 0;
    Block* current = firstBlock;
    while (current != NULL)
    {
        for (int i = 0; i < HEADER_SIZE; ++i) 
        {
            result[sPos++] = 'm';
        }

        char ch = 'f';
        if(!current->isFree()) {
            ch = 'u';
        }

        for (int i = 0; i < current->getRealSize(); ++i) 
        {
            result[sPos++] = ch;
        }
        current = current->next();
    }
    return result;
}