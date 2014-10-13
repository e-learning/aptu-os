#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "heap_functions.h"

inline unsigned short get_block_size(const struct Heap *heap, struct Block *block, struct Block *nextBlock)
{
    if(nextBlock)
        return nextBlock->offset - block->offset - sizeof(struct Block);
    else
        return heap->size - block->offset;
}

void init(struct Heap *heap, unsigned short size)
{
    heap->memory = malloc(size);
    heap->size = size;

    //heap->usedBlocks = NULL;

    heap->blocksList = (struct Block*)heap->memory;
    //heap->blocksList->size = size - sizeof(struct Block);
    heap->blocksList->offset = sizeof(struct Block);
    heap->blocksList->isFree = 1;
    heap->blocksList->next = NULL;
}

short heap_allocate(struct Heap *heap, unsigned short size)
{
    struct Block *currentBlock;
    currentBlock = heap->blocksList;
    while(currentBlock)
    {
        if(currentBlock->isFree)
        {
            //вот тут можно порефакторить слегка - под (*) одинаковый код
            unsigned short block_size;
            block_size=get_block_size(heap, currentBlock, currentBlock->next);

            if (block_size >= size && block_size <= size+ sizeof(struct Block)) {
                //новый блок создавать не нужно, т.к. оставшейся части нет или она будет бесполезна
                currentBlock->isFree = 0; //(*)
                return currentBlock->offset;
            }
            else if (block_size > size)
            {
                int newBlockOffset;
                struct Block *newBlock;
                newBlockOffset = currentBlock->offset+size;

                newBlock = (struct Block*)(heap->memory+newBlockOffset);
                newBlock->offset = newBlockOffset+ sizeof(struct Block);
                newBlock->isFree = 1;
                newBlock->next = currentBlock->next;

                currentBlock->isFree = 0;//(*)
                currentBlock->next = newBlock;
                return currentBlock->offset;
            }
        }
        currentBlock = currentBlock->next;
    }
    return -1;
}

short heap_free(struct Heap *heap, unsigned short offset)
{
    struct Block *currentBlock, *currentParentBlock;
    currentBlock = heap->blocksList;
    currentParentBlock = NULL;
    while(currentBlock)
    {
        if(currentBlock->offset == offset)
        {
            if(!currentParentBlock || !currentParentBlock->isFree)
            {
                //слева нет свободного блока, новый свободный блок будет в текущем
                currentBlock->isFree=1;
                if(currentBlock->next && currentBlock->next->isFree)
                {
                    currentBlock->next = currentBlock->next->next;
                }
            }
            else
            {
                //слева есть свободный блок, и всё будет в нём
                currentParentBlock->isFree=1;
                if(currentBlock->next && currentBlock->next->isFree)
                {
                    currentParentBlock->next = currentBlock->next->next;
                }
                else
                {
                    currentParentBlock->next = currentBlock->next;
                }
            }
            return 0;
        }
        currentParentBlock = currentBlock;
        currentBlock = currentBlock->next;
    }
    return -1;
}

const char* heap_info(struct Heap *heap)
{
    // количество занятых блоков, суммарный объём занятых блоков, наибольший свободный блок
    int usedCount=0, usedSize=0, maxFreeSize=0;
    static char result[32];
    struct Block *currentBlock = heap->blocksList;
    while(currentBlock)
    {
        if(currentBlock->isFree)
        {
            int block_size;
            if((block_size=get_block_size(heap, currentBlock, currentBlock->next))>maxFreeSize)
                maxFreeSize = block_size;
        }
        else
        {
            ++usedCount;
            usedSize+= get_block_size(heap, currentBlock, currentBlock->next);
        }
        currentBlock = currentBlock->next;
    }
    sprintf(result, "%d %d %d", usedCount, usedSize, maxFreeSize);
    return result;
}
const char* heap_map(struct Heap *heap)
{
    struct Block *currentBlock = heap->blocksList;
    int index;
    char *blockMap = (char*) malloc(sizeof(struct Block));
    char *result = (char*) malloc(heap->size+1);
    for(index=0; index<sizeof(struct Block); ++index)
        blockMap[index] = 'm';
    while(currentBlock)
    {
        int blockLimit;
        int offset = currentBlock->offset;
        memcpy(result+(offset-sizeof(struct Block)), blockMap, sizeof(struct Block));
        if(currentBlock->next)
            blockLimit = currentBlock->next->offset - sizeof(struct Block);
        else
            blockLimit = heap->size;
        for(index=offset; index < blockLimit; ++index)
        {
            if(currentBlock->isFree) result[index]='f';
            else result[index]='u';
        }
        currentBlock = currentBlock->next;
    }
    result[heap->size]='\0';
    free(blockMap);
    return result;
}

void destroy(struct Heap *heap)
{
    free(heap->memory);
}