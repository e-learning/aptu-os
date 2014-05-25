#ifndef BLOCKHEADERPROCESSOR_H
#define BLOCKHEADERPROCESSOR_H

#include <array>

using std::array;

struct BlockHeader {
    int size;
    bool is_free;
};

const unsigned int HEADER_SIZE_BYTES = 2;
typedef array<char, HEADER_SIZE_BYTES> RawHeader;

class HeaderProcessor {
public:
    static RawHeader createHeader(BlockHeader bh) {
        int combined = (bh.size << 1) | bh.is_free;
        RawHeader header;
        header[0] = combined % 256;
        combined /= 256;
        header[1] = combined % 256;
        return header;
    }

    static BlockHeader readHeader(RawHeader const& h) {
        bool is_free = h[0] & 1;
        int size = ((unsigned char) h[1]) << 8;
        size += (unsigned char) h[0];
        size >>= 1;
        return BlockHeader { size, is_free };
    }
private:
};

#endif // BLOCKHEADERPROCESSOR_H
