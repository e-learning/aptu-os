#include "bitset.h"

BitSet& BitSet::operator =(BitSet const& bs) {
    if(this != &bs) {
        p_size = bs.p_size;
        p_buffer = bs.p_buffer;
        p_set_bits = bs.p_set_bits;
    }
    return *this;
}

void BitSet::Set(size_t pos) {
    size_t byte_num = pos / 8;
    size_t rest = pos % 8;
    char byte = 1 << rest;
    p_buffer.at(byte_num) |= byte;
    ++p_set_bits;
}

void BitSet::Unset(size_t pos) {
    size_t byte_num = pos / 8;
    size_t rest = pos % 8;
    char byte = 1 << rest;
    byte = ~byte;
    p_buffer.at(byte_num) &= byte;
    --p_set_bits;
}

bool BitSet::Get(size_t pos) {
    size_t byte_num = pos / 8;
    size_t rest = pos % 8;
    char bit = p_buffer.at(byte_num) >> rest;
    return bit & 1;
}

size_t BitSet::FirstUnset() {
    for(size_t i = 0; i != p_size; ++i) {
        if(!Get(i)) {
            return i;
        }
    }
    return p_size;
}
