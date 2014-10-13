#pragma once
struct Block
{
    unsigned short isFree;
    unsigned short offset; //смещение именно данных, а не блока
    struct Block *next;
};

struct Heap
{
    void *memory;
    unsigned short size;
    struct Block *blocksList;
    struct Block *freeBlocks;
    struct Block *usedBlocks;
};

void init(struct Heap *heap, unsigned short size);
short heap_allocate(struct Heap *heap, unsigned short size);
short heap_free(struct Heap *heap, unsigned short offset);
const char* heap_info(struct Heap *heap);
const char* heap_map(struct Heap *heap);
void destroy(struct Heap *heap);
