#include "memory.h"

void memory::init_gdt()
{
	int buffer = 0;
	std::string string_buf = "";
	std::cin >> buffer;
	m_gdt.reserve(buffer);
	for (int i = 0; i < buffer; i++) 
	{
		std::cin >> string_buf;
		m_gdt.push_back(string_to_hex(string_buf));		
	}
}

void memory::init_ldt()
{
	int buffer = 0;
	std::string string_buf = "";
	std::cin >> buffer;
	m_ldt.reserve(buffer);
	for (int i = 0; i < buffer; i++) 
	{
		std::cin >> string_buf;
		m_ldt.push_back(string_to_hex(string_buf));		
	}	
}

void memory::init_page_directory()
{
	int buffer = 0;
	std::string string_buf = "";
	std::cin >> buffer;
	m_page_directory.reserve(buffer);
	for (int i = 0; i < buffer; i++) 
	{
		std::cin >> string_buf;
		m_page_directory.push_back(string_to_hex(string_buf));		
	}	
}

void memory::init_page_table()
{
	int buffer = 0;
	std::string string_buf = "";
	std::cin >> buffer;
	m_page_table.reserve(buffer);
	for (int i = 0; i < buffer; i++) 
	{
		std::cin >> string_buf;
		m_page_table.push_back(string_to_hex(string_buf));		
	}	
}

void memory::init_from_stdin()
{
	std::string string_buf;
	std::cin >> string_buf;
	m_selector = string_to_hex(string_buf);
	init_gdt();
	init_ldt();
	init_page_directory();
	init_page_table();
}
	
void memory::out()
{
	std::cout << std::hex << "Selector: " << std::hex << m_selector << std::endl;
	std::cout << "GDT" << std::endl;
	for (auto item : m_gdt) 
	{	
		std::cout << std::hex << item << std::endl;
	}
	std::cout << "LDT" << std::endl;
	for (auto item : m_ldt) 
	{	
		std::cout << std::hex << item << std::endl;
	}
	std::cout << "PD" << std::endl;
	for (auto item : m_page_directory) 
	{	
		std::cout << std::hex << item << std::endl;
	}
	std::cout << "PT" << std::endl;
	for (auto item : m_page_table) 
	{	
		std::cout << std::hex << item << std::endl;
	}
}

std::string memory::physical_address(std::string const& input)
{
	ulong offset = string_to_hex(input);
	TABLE table = get_table_from_selector(m_selector);
	std::vector<ulong> descriptor_table = table == LDT 
		? m_ldt
		: m_gdt;
	ulong privelege = privilege_from_selector(m_selector);
	ulong index = index_from_selector(m_selector);
	if (descriptor_table.size() < index || (table == GDT && index == 0))
		return "INVALID";

	ulong descriptor = descriptor_table[index];
	ulong ddpl = dpl(descriptor);
	if (ddpl > privelege)
		return "INVALID";

	if (offset >= limit(descriptor))
		return "INVALID";

	ulong linear_address = (base(descriptor) << 32) | offset;
	ulong dir = bits(linear_address, 22, 32);
	ulong table_entry = bits(linear_address, 12, 22);
	ulong result_low = linear_address & nbits(12);

	if (dir >= m_page_directory.size())
		return "INVALID";

	if (table_entry >= m_page_table.size())
		return "INVALID";

	if (!(m_page_table[table_entry] & 1) || !(m_page_directory[dir] & 1)) //Figure 4.4
		return "INVALID";

	if (m_page_directory[dir] & (1 << 7))
		if (bits(m_page_directory[dir], 17, 21))
			return "INVALID";

	ulong result_high = bits(m_page_table[table_entry], 12, 32) << 12;
	ulong result = result_high + result_low;
	if (result > nbits(32))
		return "INVALID";

	return hex_to_string(result);
}

