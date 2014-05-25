#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <vector>
#include <string>


class Address
{
    std::vector<char> hexs_;

public:
    Address();
	Address(size_t address, size_t length);
    Address(Address const & address);
    Address(std::vector<char> const & hexs);
    Address(std::string const & hex_str);
	Address& operator=(Address const & address);
    Address operator+(Address const & address) const;

    size_t get_bit(size_t bit_number) const;
    size_t get_bitcount() const;
    size_t get_value() const;
    size_t get_value(size_t start_bit, size_t length) const;

    void print() const;
};


class HexTable
{
    std::vector<Address> records_;

public:
	HexTable();

    void add_record(std::string const & address);
	Address get(size_t record_number) const;
	size_t get_size() const;
};

#endif // VIRTUAL_MEMORY_H
