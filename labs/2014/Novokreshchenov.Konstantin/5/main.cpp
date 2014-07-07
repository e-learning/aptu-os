#include "virtual_memory.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

static const std::string ERROR_MSG = "INVALID";

static void read_hex_table(size_t records_count, HexTable & table)
{
    for (size_t i = 0; i < records_count; ++i) {
        std::string str_hex_address;
        std::cin >> str_hex_address;
        table.add_record(str_hex_address);
    }
}

static Address get_segment_descriptor(Address const & selector, HexTable const & gdt, HexTable const & ldt)
{
	bool is_ldt = selector.get_bit(2);
	size_t index = selector.get_value(3, 13);

	if ((!is_ldt && (index == 0 || index >= gdt.get_size()))
		|| (is_ldt && index >= ldt.get_size())) {
			throw std::string(ERROR_MSG);
		}

	return is_ldt ? ldt.get(index) : gdt.get(index);
}

static Address get_linear_address(Address const & segment_descriptor, Address const & offset)
{
	size_t base_address = segment_descriptor.get_value(16, 16) 
		+ (segment_descriptor.get_value(32, 8) << 16) 
		+ (segment_descriptor.get_value(56, 8) << 24);

	Address linear_address(base_address + offset.get_value(), 8);
	return linear_address;
}

int main()
{
    //freopen("input.txt", "r", stdin);

	std::string str_offset;
    std::string str_SS;
    size_t records_count = 0;

	std::cin >> str_offset;
	Address offset(str_offset);

    std::cin >> str_SS;
    Address selector(str_SS);

    HexTable gdt;
    std::cin >> records_count;
    read_hex_table(records_count, gdt);

    HexTable ldt;
    std::cin >> records_count;
    read_hex_table(records_count, ldt);

    HexTable pd;
    std::cin >> records_count;
    read_hex_table(records_count, pd);

    HexTable pt;
    std::cin >> records_count;
    read_hex_table(records_count, pt);

	try {
		Address segment_descriptor = get_segment_descriptor(selector, gdt, ldt);
		if (!segment_descriptor.get_bit(15)) {
			throw std::string(ERROR_MSG);
		}

		Address linear_address = get_linear_address(segment_descriptor, offset);

		size_t directory_entry_number = linear_address.get_value(22, 10);
		size_t page_table_entry_number = linear_address.get_value(12, 10);
		if ((directory_entry_number >= pd.get_size())
			|| (page_table_entry_number >= pt.get_size())) {
			throw std::string(ERROR_MSG);
		}
		Address directory_entry(pd.get(directory_entry_number));
		Address page_table_entry(pt.get(page_table_entry_number));
		if (!directory_entry.get_bit(0)
			|| !page_table_entry.get_bit(0)) {
			throw std::string(ERROR_MSG);
		}

		size_t offset_into_page = linear_address.get_value(0, 12);
		size_t physical_address_value = (page_table_entry.get_value(12, 20) << 12) + offset_into_page;
		Address physical_address(physical_address_value, 8);
		physical_address.print();
	}
	catch (std::string & msg) {
		std::cout << msg << std::endl;
	}

    return 0;
}

