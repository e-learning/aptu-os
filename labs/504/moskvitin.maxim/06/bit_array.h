#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <cstdint>

class bit_array
{
    uint8_t *data;
public:
    bit_array(uint64_t len);
    ~bit_array();

    bool get(uint64_t pos);
    void set(uint64_t pos, bool val);
};

#endif // BIT_ARRAY_H
