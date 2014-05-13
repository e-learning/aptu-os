#ifndef BITSET_H
#define BITSET_H

#include <vector>
#include <cstdlib>

class BitSet {
public:
    BitSet()
        : p_size(0)
        , p_buffer(0)
        , p_set_bits(0)
    {}
    BitSet(size_t sz)
        : p_size(sz)
        , p_set_bits(0)
    {
        size_t bytes = sz % 8 == 0 ? sz / 8 : sz / 8 + 1;
        p_buffer.resize(bytes, 0);
    }
    BitSet(size_t sz, std::vector<char> const& bits, size_t set_bits)
        : p_size(sz)
        , p_buffer(bits)
        , p_set_bits(set_bits)
    {}

    BitSet& operator=(BitSet const& bs);

    void Set(size_t pos);
    void Unset(size_t pos);
    bool Get(size_t pos);
    size_t FirstUnset();
    size_t UnsetBits() { return p_size - p_set_bits; }

    void Resize(size_t sz) {
        p_size = sz;
        size_t bytes = (sz % 8 == 0) ? sz / 8 : sz / 8 + 1;
        p_buffer.resize(bytes, 0);
    }

    size_t size() const { return p_size; }
    std::vector<char>& Bytes() { return p_buffer; }
    void Init(size_t size, size_t set_bits) {
        p_size = size;
        p_set_bits = set_bits;
    }

private:
    size_t p_size;
    std::vector<char> p_buffer;
    size_t p_set_bits;
};
#endif // BITSET_H
