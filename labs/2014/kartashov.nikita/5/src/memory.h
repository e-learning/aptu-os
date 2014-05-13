#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <vector>
#include <iostream>

#include "selector.h"
#include "descriptor.h"
#include "utils.h"


class memory
{
	public:
		void init_from_stdin();
		std::string physical_address(std::string const& input);
		void out();

	private:
		void init_gdt();
		void init_ldt();
		void init_page_directory();
		void init_page_table();

		ulong m_selector;
		std::vector<ulong> m_gdt;
		std::vector<ulong> m_ldt;
		std::vector<ulong> m_page_directory;
		std::vector<ulong> m_page_table;
};

#endif /* end of include guard */
