#include <iostream>
#include <stdint.h>
#include <exception>
#include <vector>
#include <iomanip>

using namespace std;

struct InvalidException : public exception
{
	const char* what () const throw ()
	{
		return "INVALID";
	}
};


struct logical_address
{
	uint32_t offset;
	uint16_t seg_selector;
};

struct linear_address
{
	uint16_t offset; // but only 12 bits use
	uint16_t table; // but only 10 bits use
	uint16_t directory; // but only 10 bits use
};


void log_addr_to_lin_addr(logical_address &log_addr,
                          vector<uint64_t> GDT,
                          vector<uint64_t> LDT,
                          linear_address &lin_addr)
{
	vector<uint64_t> & DT = ((log_addr.seg_selector & 0x4) == 0) ? GDT : LDT;
	uint16_t index = log_addr.seg_selector >> 3;
	if (index >= DT.size() || (DT == GDT && index == 0))
	{
		cout << "Exc: " << 41 << endl;
		cout << "seg selector " << log_addr.seg_selector << endl;
		cout << "index " << index << endl;
		throw InvalidException();
	}
	
	uint64_t seg_descriptor = DT[index];
	cout << "index " << index << endl;
	cout << "DT[index] " << seg_descriptor << endl;
	if ((seg_descriptor & 0x800000000000) == 0) // segment-present flag
	{
		cout << "Exc: " << 48 << endl;
		throw InvalidException();
	}
	
	uint32_t seg_limit = (seg_descriptor & 0xFFFF) | ((seg_descriptor & 0xF000000000000) >> 32);
	if ((seg_descriptor & 0x80000000000000) != 0) // granularity flag
		seg_limit <<= 12;
	cout << "seg_limit " << hex << seg_limit << endl;
	if (log_addr.offset > seg_limit)
	{
		cout << "Exc: " << 57 << endl;
		throw InvalidException();
	}
	
	uint32_t base_addr = (seg_descriptor & 0xFFFF0000) >> 16;
	
	uint32_t full_lin_addr = base_addr + log_addr.offset;
	cout << "full_lin_addr " << hex << full_lin_addr << endl; 
	lin_addr.directory = (full_lin_addr & 0xFFC00000) >> 22;
	lin_addr.table = (full_lin_addr & 0x3FF000) >> 12;
	lin_addr.offset = full_lin_addr & 0x3FF;
	
	cout << "Linear address 0: " << hex << lin_addr.directory << ' ' << lin_addr.table << ' ' << lin_addr.offset  << endl;
		
	return;
}

void lin_addr_to_phys_addr(linear_address &lin_addr, 
                           vector<uint64_t> page_directory,
                           vector<uint64_t> page_table,
                           uint32_t &phys_addr)
{
	if ((page_directory[lin_addr.directory] & 0x1) == 0 ||
	    (page_table[lin_addr.table] & 0x1) == 0)
	{
		cout << "Exc: " << 78 << endl;
		throw InvalidException();
	}
	
	cout << "page frame: " << (page_table[lin_addr.table] & 0xFFFFF000) << endl;
	phys_addr = (page_table[lin_addr.table] & 0xFFFFF000) | lin_addr.offset;
	return;
}

int main()
{
	try
	{
		logical_address log_addr;
		linear_address lin_addr;
		
		cin >> hex >> log_addr.offset;
		cin >> hex >> log_addr.seg_selector;
		
		uint16_t GDT_size;
		cin >> dec >> GDT_size;
		vector<uint64_t> GDT(GDT_size);
		for (int i = 0; i < GDT_size; ++i)
			cin >> hex >> GDT[i];
		
		uint16_t LDT_size;
		cin >> dec >> LDT_size;
		vector<uint64_t> LDT(LDT_size);
		for (int i = 0; i < LDT_size; ++i)
			cin >> hex >> LDT[i];
		
		uint16_t /* ???? */ page_directory_size;
		cin >> dec >> page_directory_size;
		vector<uint64_t> page_directory(page_directory_size);
		for (int i = 0; i < page_directory_size; ++i)
			cin >> hex >> page_directory[i];
		
		uint16_t /* ???? */ page_table_size;
		cin >> dec >> page_table_size;
		vector<uint64_t> page_table(page_table_size);
		for (int i = 0; i < page_table_size; ++i)
			cin >> hex >> page_table[i];
		
		log_addr_to_lin_addr(log_addr, GDT, LDT, lin_addr);
		cout << "Linear address: " << hex << lin_addr.directory << ' ' << lin_addr.table << ' ' << lin_addr.offset  << endl;
		
		uint32_t phys_addr;
		lin_addr_to_phys_addr(lin_addr, page_directory, page_table, phys_addr);
		
		cout << hex <<  setfill('0') << setw(8) << phys_addr << endl;
		
	}
	catch (InvalidException &exc)
	{
		cout << exc.what() << endl;
	}
	
	return 0;
} 
