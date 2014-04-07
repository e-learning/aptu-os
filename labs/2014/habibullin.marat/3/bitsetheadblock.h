#ifndef BITSETHEADBLOCK_H
#define BITSETHEADBLOCK_H

#include "chainableblock.h"

class BitSetHeadBlock: public ChainableBlock {
public:
    BitSetHeadBlock(size_t s)
        : ChainableBlock(s, MAGIC)
    {}

    bool SetSize(size_t size) { return SetSizeT(size, BSSIZE_OFFS); }
    size_t GetSize() { return GetSizeT(BSSIZE_OFFS); }
    bool SetNumOfSetBits(size_t num) { return SetSizeT(num, SETBITS_OFFS); }
    size_t GetNumOfSetBits() { return GetSizeT(SETBITS_OFFS);  }

private:
    const char MAGIC = 'b';
    const size_t BSSIZE_OFFS = CHHEADER_SIZE;
    const size_t SETBITS_OFFS = BSSIZE_OFFS + SIZET_SIZE;
};

#endif // BITSETHEADBLOCK_H
