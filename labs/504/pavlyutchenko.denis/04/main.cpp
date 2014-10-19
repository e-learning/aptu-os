#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <string>

using namespace  std;

const string INVALID = "INVALID";

uint64_t translate_logical_adress_to_linear_adress(const uint64_t logical_adress, uint64_t selector, vector<uint64_t> const& gdt, vector<uint64_t> const& ldt)
{
    if (selector == 0)
    {
        cout << INVALID << endl;
        exit(0);
    }

    uint64_t table_indicator = (selector & 0x4);
    vector<uint64_t> const &descriptor_table = (table_indicator == 1) ? ldt : gdt;

    size_t descriptor_index = static_cast<size_t>(selector >> 3);

    if (descriptor_index >= descriptor_table.size())
    {
        cout << INVALID << endl;
        exit(0);
    }

    uint64_t descriptor = descriptor_table[descriptor_index];

    bool descriptor_present = (descriptor & 0x800000000000) != 0;

    if (!descriptor_present){
        cout << INVALID << endl;
        exit(0);
    }

    uint64_t segment_limit = ((descriptor & 0xF000000000000) >> 32) | (descriptor & 0xFFFF);

    bool granularity = (descriptor & 0x80000000000000) != 0;
    if (granularity)
    {
        segment_limit *= 0x1000;
    }

    if (logical_adress > segment_limit){
        cout << INVALID << endl;
        exit(0);
    }

    uint64_t base_adress = ((descriptor >> 16) & 0xFFFFFF) | ((descriptor >> (32 + 24)) & 0xFF000000);

    return logical_adress + base_adress;
}

uint64_t translate_linear_adress_to_physical_adress(uint64_t linear_adress, vector<uint64_t> const& page_directory, vector<uint64_t> const &page_table)
{
    size_t page_directory_index = static_cast<size_t>(linear_adress >> 22);

    if (page_directory_index >= page_directory.size())
    {
        cout << INVALID << endl;
        exit(0);
    }

    uint64_t PDE = (page_directory[page_directory_index] & 1);

    if (PDE == 0)
    {
        cout << INVALID << endl;
        exit(0);
    }

    size_t page_table_index = static_cast<size_t>((linear_adress >> 12) & 0x3FF);

    if (page_table_index >= page_directory.size())
    {
        cout << INVALID << endl;
        exit(0);
    }

    uint64_t PTE = (page_table[page_table_index] & 1);

    if (PTE == 0)
    {
        cout << INVALID << endl;
        exit(0);
    }

    return (linear_adress & 0xFFF) | (page_table[page_table_index] & ~0xFFF);
}

uint64_t read_logical_adress()
{
    uint64_t logical_adress;
    cin >> hex >> logical_adress;

    return logical_adress;
}

uint64_t read_selector()
{
    uint64_t selector;
    cin >> hex >> selector;

    return selector;
}

size_t read_GTD_count()
{
    size_t GTD_count;
    cin >> dec >> GTD_count;

    return GTD_count;
}

vector<uint64_t> read_GTD_table(size_t gtd_table_size)
{
    vector<uint64_t> GTD_table;

    for (size_t i = 0; i < gtd_table_size; ++i)
    {
        uint64_t a;
        cin >> hex >> a;

        GTD_table.push_back(a);
    }

    return GTD_table;
}

size_t read_LDT_count()
{
    size_t LDT_count;
    cin >> dec >> LDT_count;

    return LDT_count;
}

vector<uint64_t> read_LDT_table(size_t ldt_table_size)
{
    vector<uint64_t> LDT_table;

    for (size_t i = 0; i < ldt_table_size; ++i)
    {
        uint64_t a;
        cin >> hex >> a;

        LDT_table.push_back(a);
    }

    return LDT_table;
}

size_t raed_page_directory_size()
{
    size_t page_directory_size;
    cin >> dec >> page_directory_size;

    return page_directory_size;
}

vector<uint64_t> read_page_directory(size_t page_directory_size)
{
    vector<uint64_t> page_directory;

    for (size_t i = 0; i < page_directory_size; ++i){
        uint64_t a;
        cin >> hex >> a;

        page_directory.push_back(a);
    }

    return page_directory;
}

size_t read_page_table_size()
{
    size_t page_table_size;
    cin >> dec >> page_table_size;

    return page_table_size;
}

vector<uint64_t> read_page_table(size_t page_table_size)
{
    vector<uint64_t> page_table;

    for (size_t i = 0; i < page_table_size; ++i)
    {
        uint64_t a;
        cin >> hex >> a;

        page_table.push_back(a);
    }

    return page_table;
}

int main()
{
    uint64_t logical_adress = read_logical_adress();

    uint64_t selector = read_selector();

    size_t GTD_count = read_GTD_count();

    vector<uint64_t> GTD_table = read_GTD_table(GTD_count);

    size_t LDT_count = read_LDT_count();

    vector<uint64_t> LDT_table = read_LDT_table(LDT_count);

    size_t page_directory_size = raed_page_directory_size();

    vector<uint64_t> page_directory = read_page_directory(page_directory_size);

    size_t page_table_size = read_page_table_size();

    vector<uint64_t> page_table = read_page_table(page_table_size);

    uint64_t linear_adress = translate_logical_adress_to_linear_adress(logical_adress, selector, GTD_table, LDT_table);
    uint64_t physical_adress = translate_linear_adress_to_physical_adress(linear_adress, page_directory, page_table);

    cout << hex << physical_adress << endl;
}
