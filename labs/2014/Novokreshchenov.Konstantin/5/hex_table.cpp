#include "virtual_memory.h"

HexTable::HexTable()
    : records_(0)
{}

void HexTable::add_record(const std::string & str_address)
{
    Address address(str_address);
    records_.push_back(address);
}

Address HexTable::get(size_t record_number) const
{
	return records_[record_number];
}

size_t HexTable::get_size() const
{
	return records_.size();
}
