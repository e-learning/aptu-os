#ifndef ALLOCATOR_H
#define	ALLOCATOR_H

#include <stdlib.h>
#include <cstddef>

#define ALLOCATOR_OK 1
#define ALLOCATOR_ERROR -1

typedef ptrdiff_t mem_offset_t;

// структура, описывающая блок памяти
struct mem_control_block_s {    
    unsigned                 is_available:1; // флаг доступности блока для выделения
    mem_offset_t             offset; // смещение от начала выделяемой памяти
    size_t                   size; // размер блока
    size_t                   allocated; // байт из блока выделено пользователю
    mem_control_block_s     *next; // указатель на следубщий блок
    mem_control_block_s     *prev; // указатель на предидущий блок
    void                    *tag; // тег блока (для всех блоков равен указателю на начало распределяемой памяти)
};

typedef mem_control_block_s mem_control_block_t;

void alloc_init(size_t n);
void alloc_finit();
mem_offset_t alloc_allocate(size_t n);
int alloc_free(mem_offset_t offset);
void alloc_info();
void alloc_map();
void alloc_merge_blocks(mem_control_block_t *mcb);


#endif	/* ALLOCATOR_H */

