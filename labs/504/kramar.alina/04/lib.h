#pragma once

#include <istream>
#include <vector>

#pragma pack(1)
union segment_selector_t
{
    uint16_t value;
    struct {
        uint8_t  requested_privelege_level : 2;
        bool     table_indicator           : 1;
        uint16_t index                     : 13;
    } fields;
};

union segment_descriptor_t
{
    uint64_t value;
    struct {
        uint16_t segment_limit_1           : 16;
        uint32_t base_address_1            : 24;
        uint8_t  unused_0                  : 8;
        uint8_t  segment_limit_2           : 4;
        uint8_t  unused_1                  : 3;
        bool     granularity               : 1;
        uint8_t  base_address_2            : 8;
    } fields;

    bool is_granular() const;

    uint32_t get_base() const;

    uint32_t get_limit() const;
};

union logic_address_t
{
    uint32_t value;
    struct {
        uint32_t offset                    : 32;
    } fields;
};

union linear_address_t
{
    uint32_t value;
    struct {
        uint16_t offset                    : 12;
        uint16_t table                     : 10;
        uint16_t directory                 : 10;
    } fields;

    uint32_t get_page_directory_index() const;

    uint32_t get_page_table_index() const;

    uint32_t get_offset() const;
};

union page_directory_entry_t
{
    uint32_t value;
    struct {
        bool     present                   : 1;
        uint32_t unused                    : 31;
    } fields;

    bool is_present() const;
};

union page_table_entry_t
{
    uint32_t value;
    struct {
        bool     present                   : 1;
        uint16_t unused                    : 11;
        uint32_t physical_address          : 20;
    } fields;

    bool is_present() const;

    uint32_t get_physical_address() const;
};

#pragma pack()

template<typename T>
std::vector<T> read_table(std::istream &in, size_t size)
{
    std::vector<T> table;
    for(size_t i = 0; i < size; ++i)
    {
        T addr;
        in >> std::hex >> addr.value;
        table.push_back(addr);
    }
    return table;
}

linear_address_t logic_to_linear(logic_address_t offset, segment_descriptor_t td);

bool check_range(linear_address_t addr, segment_descriptor_t descriptor);
