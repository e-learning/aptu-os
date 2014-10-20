#include <stdint.h>
#include "page_table.h"

int is_in_gdt(unsigned short selector)
{
    return (selector & 4) == 0;
}

uint64_t get_segment_descriptor(unsigned selector, struct descriptor_table *desc_table)
{
    unsigned index = selector >> 3; //не проверяем права
    if(index < desc_table->size)
    {
        return desc_table->content[index];
    }
    else
    {
        return (uint64_t)0;
    }
}

int is_segment_present(uint64_t segment_descriptor)
{
    return (segment_descriptor & ((uint64_t)1<<47)) != 0;
}

void extract_base_limit(uint64_t segment_descriptor, unsigned *base, unsigned *limit)
{
    uint64_t long_base = 0l;
    uint64_t long_limit = 0l;

    long_base |= ( segment_descriptor & ( (uint64_t)0xff << 56 ) ) >> 32;
    long_base |= ( segment_descriptor & (uint64_t)0xffffff0000 ) >> 16;

    long_limit |= (segment_descriptor & ( (uint64_t)0xf << 48 ) ) >> 32;
    long_limit |= (segment_descriptor & (uint64_t)0xffff);

    *base = (unsigned)long_base;
    *limit = (unsigned)long_limit;
}

int is_page_granular(uint64_t descriptor)
{
    return (descriptor & ((uint64_t)1<<55)) != 0;
}

int is_expanding_down(uint64_t descriptor)
{
    return (descriptor & ((uint64_t)3<<42))>>42 == 1;
}

unsigned get_upper_bound(uint64_t descriptor) //если тип сегмента - expand-down data segment
{
    if (descriptor & ((uint64_t)1<<54))
    {
        return 0xffffffffu;
    }
    else
    {
        return 0xffffu;
    }
}

int is_pointer_in_segment(unsigned pointer, uint64_t descriptor, unsigned limit)
{
    if(is_page_granular(descriptor))
    {
        limit <<= 12;
        limit += 0xfff;
    }
    if(is_expanding_down(descriptor))
    {
        unsigned upper_bound = get_upper_bound(descriptor);
        return (pointer>limit && pointer <=upper_bound);
    }
    else
    {
        return pointer<=limit;
    }
}

void extract_pde_pte_offset(unsigned linear_addr, unsigned *page_dir_index, unsigned *page_table_index, unsigned *offset)
{
    *page_dir_index = (linear_addr & 0xffc00000u)>>22;
    *page_table_index = (linear_addr & 0x003ff000u)>>12;
    *offset = (linear_addr & 0x00000fffu);
}

int get_page_entry(struct entry_table *entry_table, unsigned page_entry_index)
{
    if(page_entry_index<entry_table->size)
    {
        return entry_table->content[page_entry_index];
    }
    else
    {
        return 0;
    }
}

int is_page_entry_present(unsigned page_entry)
{
    return page_entry & 1;
}

unsigned get_phys_addr(unsigned page_table_entry, unsigned offset)
{
    return ( page_table_entry & (0xfffff000u) ) + offset;
}

unsigned translate(unsigned pointer, unsigned short selector,
        struct entry_table *page_dir, struct entry_table *page_table,
        struct descriptor_table *gdt, struct descriptor_table *ldt)
{
    uint64_t descriptor;
    unsigned segment_base, segment_limit;
    unsigned linear_addr;
    unsigned page_dir_index, page_table_index, offset;
    unsigned page_dir_entry, page_table_entry;

    if(!selector) return 0; //нулевой селектор в GDT не используется

    if(is_in_gdt(selector))
    {
        descriptor = get_segment_descriptor(selector, gdt);
    }
    else
    {
        descriptor = get_segment_descriptor(selector, ldt);
    }
    if(!descriptor) return 0; //не удалось найти нужный дескриптор
    if(!is_segment_present(descriptor)) return 0; //сегмент выгружен из памяти

    extract_base_limit(descriptor, &segment_base, &segment_limit);

    if(!is_pointer_in_segment(pointer, descriptor, segment_limit)) return 0;//адрес не принадлежит сегменту

    linear_addr = segment_base + pointer;

    extract_pde_pte_offset(linear_addr, &page_dir_index, &page_table_index, &offset);

    page_dir_entry = get_page_entry(page_dir, page_dir_index);
    if(!page_dir_entry) return 0;//не удалось найти нужный Page Directory Entry
    if(!is_page_entry_present(page_dir_entry)) return 0; //Page Table не в памяти

    page_table_entry = get_page_entry(page_table, page_table_index);
    if(!page_table_entry) return 0;//не удалось найти нужный Page Table Entry
    if(!is_page_entry_present(page_table_entry)) return 0; //страница не в памяти

    return get_phys_addr(page_table_entry, offset);
}
