#include <string.h>
#include <stdlib.h>
#include "Vector.h"

Vector* VectorCreate(size_t elem_size)
{
    Vector *new_vector = malloc(sizeof(Vector));
    new_vector->elem_size = elem_size;
    new_vector->size = 0;
    new_vector->allocated = 0;
    return new_vector;
}

void VectorIncrease(Vector *vector)
{
    size_t new_size = vector->allocated * 2;
    if (!new_size)
        new_size = 1;
    void *new_elems = malloc(new_size * vector->elem_size);
    memcpy(new_elems, vector->elems, vector->size * vector->elem_size);
    if (vector->allocated)
        free(vector->elems);
    vector->elems = new_elems;
    vector->allocated = new_size;
}

void VectorPush(Vector *vector, void *elem)
{
    if (vector->allocated < vector->size + 1)
        VectorIncrease(vector);
    memcpy(vector->elems + vector->elem_size * vector->size, elem, vector->elem_size);
    vector->size++;
}

void* VectorGet(Vector *vector, size_t idx)
{
    if (idx >= vector->size)
        return NULL;
    return vector->elems + vector->elem_size * idx;
}

void VectorFree(Vector *vector)
{
    if (vector->allocated)
        free(vector->elems);
    free(vector);
}
