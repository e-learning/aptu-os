#ifndef UTILITY_H
#define UTILITY_H

#include <cstddef>
#include <string>
#include <vector>

void get_command (std::vector<std::string> & vCmd);
void number_to_bytes (char* buf, size_t bytesCount, size_t number);
size_t number_from_bytes (char* buf, size_t bytesCount);
void print_symbol (size_t count, char symbol);

#endif // UTILITY_H
