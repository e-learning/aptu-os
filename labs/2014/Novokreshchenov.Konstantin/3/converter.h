#ifndef CONVERTER_H
#define CONVERTER_H

#include <cstddef>
#include <string>

size_t number_from_bytes (char* bytes, size_t bytescount);
void number_to_bytes (char* bytes, size_t bytescount, size_t number);

std::string number_to_string(size_t number);
void string_to_chars(char* buf, size_t bytescount, std::string const & str);

std::string timeinfo_to_string(size_t time);

char set_bit_free (char byte, size_t pos_left);
char set_bit_busy (char byte, size_t pos_left);
size_t find_one_in_byte (char byte);

#endif // CONVERTER_H
