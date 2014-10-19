#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <cstdint>
#include <stdio.h>
#include <vector>

using namespace std;

u_int64_t get_bits_in_range(uint64_t from, int left, int right)
{
    //left is actually right and right is actually left cause of bits order
    int counts = right - left;
    return (from >> left) & (~(UINT64_MAX << counts));
}


bool get_bit_at(uint64_t from, int at)
{
     return get_bits_in_range(from, at, at + 1);
}


void input_adress_and_selector(uint64_t & logical_address, uint64_t & selector)
{
    cin >> hex >> logical_address;
    cin >> selector;
}


void input_tables(std::vector<uint64_t> & gdt_table, std::vector<uint64_t> & ldt_table,
                  std::vector<uint32_t> & page_dir, std::vector<uint32_t> & page_table)
{
    int gdt_number = 0;
    cin >> dec >> gdt_number;
    cin >> hex;
    for (int i = 0; i < gdt_number; ++i)
    {
        uint64_t buf = 0;
        cin >> buf;
        gdt_table.push_back(buf);
    }
    int ldt_number = 0;
    cin >> dec >> ldt_number;
    cin >> hex;
    for (int i = 0; i < ldt_number; ++i)
    {
        uint64_t buf = 0;
        cin >> buf;
        ldt_table.push_back(buf);
    }
    int pd_number = 0;
    cin >> dec >> pd_number;
    cin >> hex;
    for (int i = 0; i < pd_number; ++i)
    {
        uint32_t buf = 0;
        cin >> buf;
        page_dir.push_back(buf);
    }
    int page_number = 0;
    cin >> dec >> page_number;
    cin >> hex;
    for (int i = 0; i < page_number; ++i)
    {
        uint32_t buf = 0;
        cin >> buf;
        page_table.push_back(buf);
    }

}


int main()
{
    const char *invalid = "INVALID";
    uint64_t  logical_address = 0;
    uint64_t selector = 0;
    vector<uint64_t> ldt_table;
    vector<uint64_t> gdt_table;
    vector<uint32_t> page_dir;
    vector<uint32_t> page_table;

    input_adress_and_selector(logical_address, selector);
    input_tables(gdt_table, ldt_table, page_dir, page_table);

    uint16_t index = get_bits_in_range(selector, 3, 16);
    bool TI = get_bit_at(selector, 2);
    if (!TI && !index)
    {
        cout << invalid << endl;
        return -1;
    }
    uint64_t segment_descriptor = 0;
    if (TI)
    {
        segment_descriptor = ldt_table[index];
    }
    else
    {
        segment_descriptor = gdt_table[index];
    }

    bool segment_present = get_bit_at(segment_descriptor, 47);
    if (!segment_present)
    {
        cout << invalid << endl;
        return -1;
    }

    uint64_t base_address = get_bits_in_range(segment_descriptor, 16, 32) + (get_bits_in_range(segment_descriptor, 56, 64) << 24);

    uint64_t segment_limit = get_bits_in_range(segment_descriptor, 0, 16) + (get_bits_in_range(segment_descriptor, 48, 52) << 16);

    bool granularity_bit = get_bit_at(segment_descriptor, 55);
    if (granularity_bit)
    {
        segment_limit = segment_limit << 12;
    }

    if (base_address > segment_limit)
    {
        cout << invalid << endl;
        return -1;
    }

    uint64_t address = base_address + logical_address;

    uint64_t page_dir_num = get_bits_in_range(address, 22, 32);
    uint64_t page_dir_entry = page_dir[page_dir_num];
    uint64_t page_table_num = get_bits_in_range(address, 12, 22);
    uint64_t page_table_entry = page_table[page_table_num];

    bool is_valid_dir_entry = get_bit_at(page_dir_entry, 0);
    bool is_valid_table_entry = get_bit_at(page_table_entry, 0);

    if (!is_valid_table_entry && !is_valid_dir_entry)
    {
        cout << invalid << endl;
        return -1;
    }

    uint64_t offset = get_bits_in_range(address, 0, 12);
    uint64_t physical_address = get_bits_in_range(page_table_entry, 12, 32);
    physical_address = physical_address << 12;
    physical_address += offset;

    cout << hex << physical_address << endl;
    return 0;
}

