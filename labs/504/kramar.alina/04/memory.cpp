#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "lib.h"

int print_invalid()
{
    std::cout << "INVALID" << std::endl;
    return 0;
}

int print_address(uint32_t addr)
{
    std::cout << std::hex << std::setw(8) << addr << std::endl;
    return 0;
}

int main()
{
    logic_address_t logic_adress;
    std::cin >> std::hex >> logic_adress.value;

    segment_selector_t selector;
    std::cin >> std::hex >> selector.value;

    if (!selector.value)
    {
        return print_invalid();
    }

    uint32_t gdt_size;
    std::cin >> gdt_size;
    auto gdt = read_table<segment_descriptor_t>(std::cin, gdt_size);

    uint32_t ldt_size;
    std::cin >> ldt_size;
    auto ldt = read_table<segment_descriptor_t>(std::cin, ldt_size);

    size_t pages_directories_size;
    std::cin >> std::dec >> pages_directories_size;
    auto pages_directories = read_table<page_directory_entry_t>(std::cin, pages_directories_size);

    size_t pages_table_size;
    std::cin >> pages_table_size;
    auto pages_table = read_table<page_table_entry_t>(std::cin, pages_table_size);


    size_t index = selector.fields.index;
    auto &descriptor_table = selector.fields.table_indicator
        ? ldt
        : gdt;

    if (descriptor_table.size() <= index)
    {
        return print_invalid();
    }
    segment_descriptor_t descriptor = descriptor_table[index];

    linear_address_t linear_address =
        logic_to_linear(logic_adress, descriptor);

    if (!check_range(linear_address, descriptor))
    {
        return print_invalid();
    }

    size_t page_directory_index = linear_address.get_page_directory_index();

    if (pages_directories_size <= page_directory_index)
    {
        return print_invalid();
    }

    if (!pages_directories[page_directory_index].is_present())
    {
        return print_invalid();
    }

    size_t page_table_index = linear_address.get_page_table_index();

    if (pages_table_size <= page_table_index)
    {
        return print_invalid();
    }

    page_table_entry_t table = pages_table[page_table_index];
    if (!table.is_present())
    {
        return print_invalid();
    }

    uint32_t addr = (table.get_physical_address() << 12) + linear_address.get_offset();
    return print_address(addr);
}
