#include <stdlib.h>
#include <inttypes.h>

#include "memory.h"

#pragma pack(1)

union segment_selector_t
{
    uint16_t value;
    struct
    {
        uint8_t  requested_privelege_level  : 2;
        bool     table_indicator            : 1;
        uint16_t index                      : 13;
    } fields;
} selector;


union segment_descriptor_t
{
    uint64_t value;
    struct
    {
        uint16_t segment_limit_1            : 16;
        uint32_t base_address_1             : 24;
        uint8_t  unused_0                   : 5;
        uint8_t  privelege_level            : 2;
        uint8_t  present                    : 1;
        uint8_t  segment_limit_2            : 4;
        uint8_t  unused_1                   : 3;
        bool     granularity                : 1;
        uint8_t  base_address_2             : 8;
    } fields;
};

union linear_address_t
{
    uint32_t value;
    struct
    {
        uint16_t offset                     : 12;
        uint16_t table                      : 10;
        uint16_t directory                  : 10;
    } fields;
};

union page_table_entry_t
{
    uint32_t value;
    struct
    {
        bool     present                    : 1;
        uint16_t unused                     : 11;
        uint32_t physical_address           : 20;
    } fields;
};

#pragma pack()

struct discriptor_table_t
{
    union segment_descriptor_t* records;
    uint32_t records_count;
}global_discriptor_table, local_discriptor_table;


struct page_table_t
{
    union page_table_entry_t* records;
    uint32_t records_count;
}page_directory, page_table;


void init_discriptor_table(FILE* input, struct discriptor_table_t* dt)
{
    uint32_t i;
    fscanf(input, "%d", &(dt->records_count));
    dt->records = (union segment_descriptor_t*)calloc(dt->records_count,
                                                    sizeof(union segment_descriptor_t));
    for (i = 0; i < dt->records_count; i++)
    {
        fscanf(input, "%"SCNx64"", &(dt->records[i].value));
    }
}


void deinit_descriptor_table(struct discriptor_table_t* dt)
{
    free(dt->records);
    dt->records = NULL;
    dt->records_count = 0;
}


void init_paging_table(FILE* input, struct page_table_t* pt)
{
    uint32_t i;
    fscanf(input, "%d", &(pt->records_count));
    pt->records = (union page_table_entry_t*)calloc(pt->records_count,
                                              sizeof(union page_table_entry_t));
    for (i = 0; i < pt->records_count; i++)
    {
        fscanf(input, "%x", &(pt->records[i].value));
    }
}


void deinit_paging_table(struct page_table_t* pt)
{
    free(pt->records);
    pt->records = NULL;
    pt->records_count = 0;
}



void mem_init(FILE* input)
{
    fscanf(input, "%hx", &selector.value);
    init_discriptor_table(input, &global_discriptor_table);
    init_discriptor_table(input, &local_discriptor_table);
    init_paging_table(input, &page_directory);
    init_paging_table(input, &page_table);
}


void mem_deinit()
{
    deinit_descriptor_table(&global_discriptor_table);
    deinit_descriptor_table(&local_discriptor_table);
    deinit_paging_table(&page_directory);
    deinit_paging_table(&page_table);
}


uint32_t segment_descriptor_get_base(union segment_descriptor_t self)
{
    uint32_t begin = self.fields.base_address_2;
    uint32_t end = self.fields.base_address_1;
    return (begin << 8) | end;
}


uint32_t segment_descriptor_get_limit(union segment_descriptor_t self)
{
    uint32_t begin = self.fields.segment_limit_2;
    uint32_t end = self.fields.segment_limit_1;
    return (begin << 4) | end;
}


bool segment_descriptor_check_range(union segment_descriptor_t self, uint32_t addr)
{
    uint32_t limit_multiplier = self.fields.granularity ? (1 << 12) : 1;
    uint32_t lower = segment_descriptor_get_base(self);
    uint32_t upper = lower + segment_descriptor_get_limit(self) * limit_multiplier;
    return lower <= addr && addr <= upper;
}


int32_t to_linear_address(uint32_t logical_address, uint32_t* linear_address)
{
    struct discriptor_table_t* dt;
    if (selector.fields.table_indicator)
    {
        dt = &local_discriptor_table;
    }
    else
    {
        dt = &global_discriptor_table;
        if (selector.fields.index == 0) // if gdt zero line not used
        {
            return -1;
        }
    }

    uint32_t index = selector.fields.index;
    if (dt->records_count <= index)
    {
        return -1;
    }


    union segment_descriptor_t descriptor = dt->records[index];
    if (!descriptor.fields.present)
    {
        return -1; //segment not presented
    }

    if (descriptor.fields.privelege_level < selector.fields.requested_privelege_level)
    {
        return -1; // access error
    }

    *linear_address = segment_descriptor_get_base(descriptor) + logical_address;
    if (!segment_descriptor_check_range(descriptor, *linear_address))
    {
        return -1; // offset out of range
    }
    return 0;
}


int32_t to_physical_address(uint32_t linear_address, uint32_t* physical_address)
{
    union linear_address_t la;
    la.value = linear_address;

    uint32_t page_directory_index = la.fields.directory;
    if (page_directory.records_count <= page_directory_index)
    {
        return -1; // page directory index out of range
    }

    union page_table_entry_t dir_record = page_directory.records[page_directory_index];
    if (!dir_record.fields.present)
    {
        return -1; // page deirectory record not presented
    }

    uint32_t page_table_index = la.fields.table;
    if (page_table.records_count <= page_table_index)
    {
        return -1; // page table index out of range
    }

    union page_table_entry_t table_record = page_table.records[page_table_index];
    if (!table_record.fields.present)
    {
        return -1; // page table record not presented
    }

    *physical_address = (table_record.fields.physical_address << 12) + la.fields.offset;
    return 0;
}


int32_t mem_translate_address(uint32_t logical_address, uint32_t* physical_address)
{
    uint32_t linear_address;
    if (to_linear_address(logical_address, &linear_address) != 0)
    {
        return -1;
    }
    if (to_physical_address(linear_address, physical_address) != 0)
    {
        return -1;
    }
    return 0;
}