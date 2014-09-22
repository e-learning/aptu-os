#ifndef __LS_H_
#define __LS_H_

#include <stddef.h>

struct strArray
{
    size_t size;
    size_t allocated_size;
    char **entries;
};

struct strArray* createStrArray();
int strArrayPush(struct strArray *arr, const char* entry);
void strArrayFree(struct strArray *arr);
int printAsTable(struct strArray *arr);
int ls();

#endif