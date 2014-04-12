#ifndef READER_H
#define READER_H

#include <iostream>
#include <fstream>
#include <string>

size_t read_number_from_bytes (std::fstream & fs, size_t bytescount);
std::string read_string_from_bytes (std::fstream & fs, size_t bytescount);

#endif // READER_H
