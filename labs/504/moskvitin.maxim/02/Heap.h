#ifndef __HEAP_H
#define __HEAP_H

struct Heap;

typedef struct Heap
{
    struct Heap *left, *right;
    int id;
    int key;
    int height;
} Heap;

Heap* HeapMerge(Heap *x, Heap *y);
Heap* HeapInsert(Heap *heap, int key, int id);
int HeapPop(Heap **heap);

#endif