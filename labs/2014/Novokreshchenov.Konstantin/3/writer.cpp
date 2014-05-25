#include "writer.h"
#include "converter.h"

void write_number_to_bytes (std::fstream & fs, size_t bytescount, size_t number)
{
    char* buf = new char[bytescount];
    number_to_bytes(buf, bytescount, number);
    fs.write(buf, bytescount);
    delete[] buf;
}

void write_string_to_bytes (std::fstream & fs, size_t bytescount, std::string str)
{
    char* bytes = new char[bytescount];
    string_to_chars(bytes, bytescount, str);
    fs.write(bytes, bytescount);
}

void fill_block_by_char(std::fstream & bitmapfs, size_t blocksize, char symbol)
{
    for (size_t i = 0; i < blocksize; ++i) {
        bitmapfs.put(symbol);
    }
}
