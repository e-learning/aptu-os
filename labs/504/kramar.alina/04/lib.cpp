#include <iostream>
#include "lib.h"

uint32_t segment_descriptor_t::get_base() const
{
    uint32_t begin = fields.base_address_2;
    uint32_t end = fields.base_address_1;
    return (begin << 8) | end;
}

uint32_t segment_descriptor_t::get_limit() const
{
    uint32_t begin = fields.segment_limit_2;
    uint32_t end = fields.segment_limit_1;
    return (begin << 4) | end;
}

bool segment_descriptor_t::is_granular() const
{
    return fields.granularity;
}

linear_address_t logic_to_linear(logic_address_t offset, segment_descriptor_t td)
{
    linear_address_t addr;
    addr.value = td.get_base() + offset.value;
    return addr;
}

bool check_range(linear_address_t addr, segment_descriptor_t descriptor)
{
    uint32_t limit_multiplier = descriptor.is_granular() ? (1 << 12) : 1;
    uint32_t lower = descriptor.get_base();
    uint32_t upper = lower + descriptor.get_limit() * limit_multiplier;
    return lower <= addr.value && addr.value <= upper;
}

uint32_t linear_address_t::get_page_directory_index() const
{
    return fields.directory;
}

uint32_t linear_address_t::get_page_table_index() const
{
    return fields.table;
}

uint32_t linear_address_t::get_offset() const
{
    return 0 | fields.offset;
}

bool page_table_entry_t::is_present() const
{
    return fields.present;
}

uint32_t page_table_entry_t::get_physical_address() const
{
    return fields.physical_address;
}

bool page_directory_entry_t::is_present() const
{
    return fields.present;
}
