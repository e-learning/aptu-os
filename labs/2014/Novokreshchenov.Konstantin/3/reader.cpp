#include "reader.h"
#include "converter.h"

size_t read_number_from_bytes (std::fstream & fs, size_t bytescount)
{
    char* buf = new char[bytescount];
    fs.read(buf, bytescount);
    size_t number = number_from_bytes(buf, bytescount);
    delete[] buf;
    return number;
}

std::string read_string_from_bytes (std::fstream & fs, size_t bytescount)
{
    char* buf = new char[bytescount + 1];
    fs.read(buf, bytescount);
    buf[bytescount] = '\0';
    std::string str = buf;
    delete[] buf;
    return str;
}


