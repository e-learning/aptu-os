#include "allocator.h"
#include <malloc.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <string.h>

using namespace std;

void   *managed_memory_start; // указатель на начало распределяемой памяти

// функция инициализации распределяемой памяти
void alloc_init(size_t n)
{
    mem_control_block_t *mcb;
    size_t full_size;
    
    // нужно выделеить память под описатель начального блока и n байт, которые запросил для распределения пользователь
    full_size = n + sizeof(mem_control_block_t);
    managed_memory_start = malloc(full_size);
    memset(managed_memory_start, 0 , full_size);
    
    // инициализируем начальный описатель блока
    mcb = (mem_control_block_t *) managed_memory_start;
    mcb->is_available = 1;
    mcb->size = n;
    mcb->allocated = 0;
    mcb->offset = sizeof(mem_control_block_t);
    mcb->next = NULL;
    mcb->prev = NULL;
    mcb->tag = managed_memory_start;
}

// освобождаем память
void alloc_finit()
{
    free(managed_memory_start);
}

mem_offset_t alloc_allocate(size_t n)
{
    mem_control_block_t  *current_mcb;
    void                 *memory_location = NULL;
    size_t                full_size;
    
    full_size = n + sizeof(mem_control_block_t);
    
    current_mcb = (mem_control_block_t *) managed_memory_start;
    
    while(current_mcb) // проходим по списку блоков в поиске подходящего
    {
        if(current_mcb->is_available)
        {
            if(current_mcb->size >= n)
            {
                // найден доступный для распределения блок нужного размера
                // если размер "лишней" памяти блока достаточен для хранения нового описателя блока памяти,
                // разделяем текущий блок на два
                if(current_mcb->size - n > sizeof(mem_control_block_t))
                {
                    mem_control_block_t *new_mcb;
                    new_mcb = (mem_control_block_t *)((void *)current_mcb + full_size);
                    new_mcb->is_available = 1;
                    new_mcb->prev = current_mcb;
                    new_mcb->next = current_mcb->next; 
                    new_mcb->size = current_mcb->size - full_size;
                    new_mcb->allocated = 0;
                    new_mcb->offset = current_mcb->offset + full_size;
                    new_mcb->tag = managed_memory_start;
                    current_mcb->next = new_mcb;
                    current_mcb->size = n;
                }
                
                current_mcb->allocated = n;
                current_mcb->is_available = 0;
                memory_location = current_mcb + sizeof(mem_control_block_t);
                break;
            }
        }
        current_mcb = current_mcb->next;
    }
    if(memory_location)
        return current_mcb->offset;
    else
        return -1;
}

int alloc_free(mem_offset_t offset)
{
    mem_control_block_t  *current_mcb;
    current_mcb = (mem_control_block_t *) (managed_memory_start + offset - sizeof(mem_control_block_t));
    if(current_mcb->tag == managed_memory_start && !current_mcb->is_available)
    {
        current_mcb->allocated = 0;
        current_mcb->is_available = 1;
        alloc_merge_blocks(current_mcb); // объединяем смежные доступные блоки
        return ALLOCATOR_OK;
    }
    else
    {
        return ALLOCATOR_ERROR;
    }
}

void alloc_info()
{
    mem_control_block_t *current_mcb;
    size_t               max_available;
    size_t               user_memory;
    int                  user_blocks;
    
    max_available = 0;
    user_memory = 0;
    user_blocks = 0;
    current_mcb = (mem_control_block_t *) managed_memory_start;
    
    while(current_mcb)
    {
        if(!current_mcb->is_available)
        {
            user_blocks++;
            user_memory += current_mcb->allocated;
        }
        else
        {
            if(current_mcb->size > max_available)
                max_available = current_mcb->size;
        }
        
        current_mcb = current_mcb->next;
    }
    
    cout << "   КОЛИЧЕСТВО БЛОКОВ ВЫДЕЛЕННЫХ ПОЛЬЗОВАТЕЛЮ: " << user_blocks << endl;
    cout << "   СУММАРНЫЙ ОБЪЕМ ПАМЯТИ ВЫДЕЛЕННЫЙ ПОЛЬЗОВАТЕЛЮ: " << user_memory << endl;
    cout << "   МАКСИМАЛЬНЫЙ РАЗМЕР БЛОКА ПАМЯТИ, КОТОРУЮ МОЖНО ВЫДЕЛИТЬ: " << max_available << endl;
}

void alloc_map()
{
    size_t                byte_counter;
    mem_control_block_t  *current_mcb;
    
    current_mcb = (mem_control_block_t *) managed_memory_start;
    
    byte_counter = 0;
    
    cout << "КАРТА ПАМЯТИ:";
    
    while(current_mcb)
    {
        
        for(int i = 0 ; i < sizeof(mem_control_block_t) ; i++)
        {
            if((byte_counter % 50) == 0)
            {
                cout << endl << setw(10) << byte_counter << " ";
            }
            byte_counter++;
            cout << 'm';
        }
        if(current_mcb->is_available)
        {
            for(int i = 0 ; i < current_mcb->size ; i++)
            {
                if((byte_counter % 50) == 0)
                {
                    cout << endl << setw(10) << byte_counter << " ";
                }
                byte_counter++;
                cout << 'f';
            }
        }
        else
        {
            for(int i = 0 ; i < current_mcb->allocated ; i++)
            {
                if((byte_counter % 50) == 0)
                {
                    cout << endl << setw(10) << byte_counter << " ";
                }
                byte_counter++;
                cout << 'u';
            }
            for(int i = 0 ; i < current_mcb->size - current_mcb->allocated; i++)
            {
                if((byte_counter % 50) == 0)
                {
                    cout << endl << setw(10) << byte_counter << " ";
                }
                byte_counter++;
                cout << 'f';
            }
        }
        
        current_mcb = current_mcb->next;
    }
    
    cout << endl;
}


// функция объединения смежных доступных для распределения блоков в один блок
void alloc_merge_blocks(mem_control_block_t *mcb)
{
    mem_control_block_t  *current_mcb;
    mem_control_block_t  *tmp;
    
    current_mcb = mcb;
    
    // сначала пытаемся объединить текущий блок с предидущими блоками
    while(current_mcb->prev != NULL && current_mcb->prev->is_available)
    {
        current_mcb->prev->next = current_mcb->next;
        current_mcb->next->prev = current_mcb->prev;
        current_mcb->prev->size += current_mcb->size + sizeof(mem_control_block_t);
        tmp = current_mcb;
        current_mcb = current_mcb->prev;
        memset(tmp, 0, sizeof(mem_control_block_t));
    }
    
    // пытаемся объединить текущий блок с последующими блоками
    while(current_mcb && current_mcb->next != NULL && current_mcb->next->is_available)
    {
        current_mcb->size += current_mcb->next->size + sizeof(mem_control_block_t);
        tmp = current_mcb->next;
        current_mcb->next = current_mcb->next->next;
        if(current_mcb->next != NULL)
        {
            current_mcb->next->prev = current_mcb;
        }
        current_mcb = current_mcb->next;
        memset(tmp, 0, sizeof(mem_control_block_t));
    }
}
