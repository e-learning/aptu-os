#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stddef.h>

typedef struct Vector{
    size_t elem_size;
    size_t size;
    size_t allocated;
    void* elems;
} Vector;

Vector* VectorCreate(size_t elem_size);
void VectorPush(Vector *vector, void *elem);
void* VectorGet(Vector *vector, size_t idx);
void VectorFree(Vector *vector);

#endif
