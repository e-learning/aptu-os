#include <string.h>
#include <stdio.h>
#include "list.h"

void init(List* list)
{
  list->size = 0;
	
	memset(list->data,0, MAX_LIST_ITEMS* sizeof(void*));
}

void *getItem(List *list, size_t idx)
{
  if( list == NULL || idx > list->size-1 )
		return NULL;
  return list->data[idx];
}

void print(List *list)
{
  printf("list_len = %u",list->size);
}


size_t addItem(List* list, void *item)
{
  if(list->size >=MAX_LIST_ITEMS)
		return MAX_LIST_ITEMS;
	list->data[list->size++] = item;
	return list->size-1;
}


void* extract(List* list, int idx)
{
	if( list->size == 0 || idx >= list->size )
		return NULL;

  void *ret= list->data[idx];
	memmove(list->data+idx,list->data+1,(list->size-idx-1)*sizeof(void*));
	list->size--;

	return ret;

}

void* pop(List* list)
{
	return extract(list,0);
}

