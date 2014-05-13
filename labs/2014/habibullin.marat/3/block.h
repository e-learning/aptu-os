#ifndef BLOCK_H
#define BLOCK_H

#include <cstdlib>
#include <string>
#include <cstring>

class Block {
public:
    void Read(std::string const& path);
    void Write(std::string const& path);

protected:
    Block(size_t s)
        : size_(s)
        , buf_(new char[size_])
    {
        memset(buf_, 0, size_);
    }
    Block(Block const& other) {
        size_ = other.size_;
        buf_ = new char[size_];
        memcpy(buf_, other.buf_, size_);
    }
    ~Block() {
        delete[] buf_;
    }
    Block& operator=(Block const& other) {
        if(&other != this) {
            size_ = other.size_;
            delete[] buf_;
            buf_ = new char[size_];
            memcpy(buf_, other.buf_, size_);
        }
        return *this;
    }

    bool SetSizeT(size_t data, size_t offset);
    bool SetString(std::string const& data, size_t offset);
    size_t GetSizeT(size_t offset);
    std::string GetString(size_t offset, size_t str_size);
    bool SetBytes(char* bytes, size_t num, size_t offset);
    char* GetBytes(size_t num, size_t offset);

    size_t size_;
    const static size_t SIZET_SIZE = 4;
private:
    char* buf_;
};

#endif // BLOCK_H
