#include "bit_array.h"
#include <algorithm>

bit_array::bit_array(uint64_t len)
    :   data(nullptr)
{
    uint64_t byte_count = (len + 1) / 8;
    if ((len + 1) % 8)
    {
        ++byte_count;
    }
    data = new uint8_t[byte_count];
    std::fill(data, data + byte_count, 0);
}

bit_array::~bit_array()
{
    if (data)
        delete[] data;
}

bool bit_array::get(uint64_t pos)
{
    uint64_t idx = pos / 8;
    int offset = pos % 8;
    return (data[idx] & (1 << offset)) != 0;
}

void bit_array::set(uint64_t pos, bool val)
{
    uint64_t idx = pos / 8;
    int offset = pos % 8;
    if (val)
    {
        data[idx] |= (1 << offset);
    }
    else
    {
        data[idx] = ~data[idx];
        data[idx] |= (1 << offset);
        data[idx] = ~data[idx];
    }
}
