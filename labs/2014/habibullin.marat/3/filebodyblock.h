#ifndef FILEBODYBLOCK_H
#define FILEBODYBLOCK_H

#include <fstream>

#include "chainableblock.h"

class FileBodyBlock : public ChainableBlock {
public:
    FileBodyBlock(size_t s)
        : ChainableBlock(s, MAGIC)
        , size_bytes_(0)
    {}
    void SetData(std::ifstream& source) {
        size_t max_body_size = size_ - BODY_OFFS;
        char* tmpbuf = new char[max_body_size];
        source.read(tmpbuf, max_body_size);
        size_t read_bytes = source.gcount();
        SetSizeT(read_bytes, SIZEBYTES_OFFS);
        SetBytes(tmpbuf, read_bytes, BODY_OFFS);
        delete[] tmpbuf;
    }
    void SetData(char* data, size_t size_bytes) {
        SetSizeT(size_bytes, SIZEBYTES_OFFS);
        SetBytes(data, size_bytes, BODY_OFFS);
    }

    void GetData(std::ofstream& dest) {
        size_t size_bytes = GetSizeT(SIZEBYTES_OFFS);
        char* data = GetBytes(size_bytes, BODY_OFFS);
        dest.write(data, size_bytes);
        delete[] data;
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

    const char MAGIC = 'g';
    const size_t SIZEBYTES_OFFS = CHHEADER_SIZE;
    const size_t BODY_OFFS = SIZEBYTES_OFFS + SIZET_SIZE;
};

#endif // FILEBODYBLOCK_H
