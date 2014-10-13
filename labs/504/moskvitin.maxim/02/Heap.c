#include <unistd.h>
#include <stdlib.h>
#include "Heap.h"

void swap(void **x, void **y)
{
    void *tmp = *x;
    *x = *y;
    *y = tmp;
}

Heap* HeapMerge(Heap *x, Heap *y)
{
    if (!x)
        return y;
    if (!y)
        return x;
    if (x->key > y->key)
        swap((void**)&x, (void**)&y);
    x->right = HeapMerge(x->right, y);
    int right_height = 0;
    if (x->right)
        right_height = x->right->height;
    int left_height = 0;
    if (x->left)
        left_height = x->left->height;
    if (right_height > left_height)
        swap((void**)&(x->left), (void**)&(x->right));
    x->height = 1;
    if (x->right)
        x->height += x->right->height;
    return x;
}

Heap* HeapInsert(Heap *heap, int key, int id)
{
    Heap* new_heap = malloc(sizeof(Heap));
    new_heap->left = NULL;
    new_heap->right = NULL;
    new_heap->key = key;
    new_heap->id = id;
    new_heap->height = 1;
    if (!heap)
        return new_heap;
    else
        return HeapMerge(heap, new_heap);
}

int HeapPop(Heap **heap)
{
    if (!(*heap))
        return -1;
    int id = (*heap)->id;
    Heap *left = (*heap)->left;
    Heap *right = (*heap)->right;
    free(*heap);
    *heap = HeapMerge(left, right);
    return id;
}