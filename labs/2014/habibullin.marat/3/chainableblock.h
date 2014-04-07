#ifndef CHAINABLEBLOCK_H
#define CHAINABLEBLOCK_H

#include "block.h"

using std::string;

class ChainableBlock : public Block {
public:
    ChainableBlock(size_t s, char magic) : Block(s) {
        SetString(string(1, magic), MAGIC_OFFS);
    }

    void SetNext(size_t next_ptr) {
        SetSizeT(next_ptr, NEXT_OFFS);
    }
    size_t Next() { return GetSizeT(NEXT_OFFS); }
private:
    static const size_t MAGIC_OFFS = 0;
    static const size_t NEXT_OFFS = 1;
public:
    static const size_t CHHEADER_SIZE = NEXT_OFFS + SIZET_SIZE;
};

#endif // CHAINABLEBLOCK_H
