#include "virtual_memory.h"
#include <iostream>
#include <sstream>

static const size_t BITS_IN_HEX = 4;
static const int ERROR = -1;

typedef std::vector<char>::iterator v_uchar_it;

static char convert_to_byte(char const hex_char)
{
    size_t x;
    std::stringstream ss;

    ss << std::hex << hex_char;
    ss >> x;

    return char(x);
}

Address::Address()
    : hexs_(0)
{}

Address::Address(size_t address, size_t length)
{
	hexs_.resize(length);
	for (size_t i = 0; i < length; ++i) {
		hexs_[i] = (address >> 4 * i) & 0xF;
	}
}

Address::Address(std::vector<char> const & hexs)
    : hexs_(hexs)
{}

Address::Address(std::string const & hex_str)
    : hexs_(hex_str.length())
{
    for (size_t i = 0; i < hex_str.length(); ++i) {
        hexs_[hexs_.size() - i - 1] = convert_to_byte(hex_str[i]);
    }
}

Address::Address(Address const & address)
    : hexs_(address.hexs_)
{}

Address& Address::operator=(Address const & address)
{
	hexs_ = address.hexs_;

	return *this;
}

Address Address::operator+ (Address const & address) const
{
    size_t result_size = this->hexs_.size() > address.hexs_.size()
            ? this->hexs_.size()
            : address.hexs_.size();

    std::vector<char> result_hexs(result_size, 0);

    size_t carry = 0;
    for (size_t i = 0; i < result_size; ++i) {
        result_hexs[i] += carry;
        carry = result_hexs[i] / 16;
        result_hexs[i] %= 16;
    }

    Address result(result_hexs);
    return result;
}

size_t Address::get_bit(size_t bit_number) const
{
    size_t hex_number = bit_number / BITS_IN_HEX;
    size_t hex_bit = bit_number % BITS_IN_HEX;

    return (hexs_[hex_number] >> hex_bit) & 1;
}

size_t Address::get_bitcount() const
{
    return hexs_.size() * BITS_IN_HEX;
}

size_t Address::get_value() const
{
    return get_value(0, get_bitcount());
}

size_t Address::get_value(size_t start_bit, size_t length) const
{
    size_t value = 0;
    for (size_t i = 0; i < length; ++i) {
        value += (get_bit(start_bit + i) << i);
    }

    return value;
}

void Address::print() const
{
    for (size_t i = 0; i < hexs_.size(); ++i) {
        std::cout << std::hex << (size_t)hexs_[hexs_.size() - i - 1];
    }
    std::cout << std::dec << std::endl;
}
