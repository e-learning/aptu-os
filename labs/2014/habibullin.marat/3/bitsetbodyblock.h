#ifndef BITSETBODYBLOCK_H
#define BITSETBODYBLOCK_H

#include "chainableblock.h"

class BitSetBodyBlock : public ChainableBlock {
public:
    BitSetBodyBlock(size_t s)
        : ChainableBlock(s, MAGIC)
        , size_bytes_(0)
    {}

    size_t MaxCapacity() { return (size_ - BODY_OFFS); }

    void SetData(char* data, size_t size_bytes) {
        SetSizeT(size_bytes, SIZEBYTES_OFFS);
        SetBytes(data, size_bytes, BODY_OFFS);
    }
    size_t GetData(char* buffer) {
        size_t size_bytes = GetSizeT(SIZEBYTES_OFFS);
        char* data = GetBytes(size_bytes, BODY_OFFS);
        memcpy(buffer, data, size_bytes);
        delete[] data;
        return size_bytes;
    }

private:
    size_t size_bytes_;

    const char MAGIC = 'c';
    const size_t SIZEBYTES_OFFS = CHHEADER_SIZE;
    const size_t BODY_OFFS = SIZEBYTES_OFFS + SIZET_SIZE;
};

#endif // BITSETBODYBLOCK_H
