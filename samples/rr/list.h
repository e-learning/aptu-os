#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

#define MAX_LIST_ITEMS ((size_t)10000)

struct List 
{
  void*  data[MAX_LIST_ITEMS];
	size_t size;
};

size_t addItem(List* list, void *item);

// return item or NULL 
void* getItem(List *list, size_t idx);

void init(List* list);

void print(List *list);

void* pop(List* list);

void* extract(List* list, int idx);


#endif // LIST_H
