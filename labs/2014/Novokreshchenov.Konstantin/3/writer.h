#ifndef WRITER_H
#define WRITER_H

#include <iostream>
#include <fstream>
#include <string>

void write_number_to_bytes (std::fstream & fs, size_t bytescount, size_t number);
void write_string_to_bytes (std::fstream & fs, size_t bytescount, std::string str);
void fill_block_by_char(std::fstream & bitmapfs, size_t blocksize, char symbol);

#endif // WRITER_H
