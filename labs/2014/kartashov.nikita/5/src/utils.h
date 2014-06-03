#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <iomanip>

typedef unsigned long ulong;

ulong string_to_hex(std::string const& s);
std::string hex_to_string(ulong h);
ulong nbits(int n);
ulong bits(ulong source, int from, int to);

#endif /* end of include guard */
