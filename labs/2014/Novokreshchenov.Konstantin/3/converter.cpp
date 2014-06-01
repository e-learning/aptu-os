#include "Const.h"
#include "converter.h"
#include <sstream>
#include <string>
#include <cmath>
#include <ctime>

size_t number_from_bytes (char* bytes, size_t bytescount)
{
    size_t number = 0;
    for (size_t i = 0; i < bytescount; ++i) {
        number += (unsigned char)(bytes[i]) * (int)pow(2, BITS_PER_BYTE * (bytescount - i - 1));
    }
    return number;
}

void number_to_bytes (char* bytes, size_t bytescount, size_t number)
{
    for (size_t i = 0; i < bytescount; ++i) {
        bytes[bytescount - i - 1] = number >> 8 * i;
    }
}

std::string number_to_string(size_t number)
{
    std::ostringstream ss;
    ss << number;
    std::string strNumber = ss.str();
    return strNumber;
}

void string_to_chars(char* buf, size_t bytescount, std::string const & str)
{
    size_t i = 0;
    for (; i < str.size(); ++i) {
        buf[i] = str[i];
    }
    for (; i< bytescount; ++i) {
        buf[i] = '\0';
    }
}

std::string timeinfo_to_string(size_t time)
{
    time_t timer = static_cast<time_t>(time);
    tm* timeinfo = std::localtime(&timer);
    char* timebuf = new char[BYTES_FOR_TIMEINFO];
    strftime(timebuf, BYTES_FOR_TIMEINFO, "%c", timeinfo);
    std::string timestr = timebuf;
    delete[] timebuf;
    return timestr;
}

char set_bit_free (char byte, size_t pos_left) {
    return byte | (char)pow (2, BITS_PER_BYTE - 1 - pos_left);
}

char set_bit_busy (char byte, size_t pos_left) {
    return byte & (MAX_BYTE - (char)pow(2, BITS_PER_BYTE - 1 - pos_left));
}

size_t find_one_in_byte (char byte)
{
    size_t firstOne = (size_t)pow(2, BITS_PER_BYTE - 1);
    for (size_t i = 0; i < BITS_PER_BYTE; ++i) {
        if (((byte << i) & firstOne) == firstOne) {
            return i;
        }
    }
    return BITS_PER_BYTE;
}









