#include "utils.h"

ulong string_to_hex(std::string const& s)
{
	return std::stoul("0x" + s, nullptr, 16);
}

std::string hex_to_string(ulong h)
{
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(8) 
         << std::hex << h;
  return stream.str();
}

ulong nbits(int n)
{
	return (1UL << n) - 1UL;
}

ulong bits(ulong source, int from, int to)
{
	int size = to - from;
	return (source >> from) & nbits(size);
}

