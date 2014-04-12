#include "utility.h"
#include <iostream>
#include <sstream>
#include <cmath>

static size_t const BITS_PER_BYTE = 8;

void number_to_bytes(char* buf, size_t bytesCount, size_t number)
{
    for (size_t i = 0; i < bytesCount; ++i) {
        buf[bytesCount - i - 1] = number >> 8 * i;
    }
}

size_t number_from_bytes(char *buf, size_t bytesCount)
{
    size_t number = 0;
    for (size_t i = 0; i < bytesCount; ++i) {
        number += (unsigned char)(buf[bytesCount - i - 1]) * (size_t)pow(2, BITS_PER_BYTE * i);
    }
    return number;
}

void print_symbol (size_t count, char symbol)
{
    for (size_t i = 0; i < count; ++i) {
        std::cout << symbol;
    }
}

void get_command (std::vector<std::string> & vCmd)
{
    std::string strCmd;
    std::getline(std::cin, strCmd);
    std::stringstream ss(strCmd);
    std::string item;
    while (ss >> item) {
        vCmd.push_back(item);
    }
}
