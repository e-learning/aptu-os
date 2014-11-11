#pragma once

#include <streambuf>
#include <iostream>

#include "fs_t.h"

class out_file_buffer: public streambuf {
public:
    explicit out_file_buffer(fs_t*);
    virtual ~out_file_buffer();

protected:
    fs_t* fs_;
    uint32_t current_block_;

    int buffer_size_;
    char* buffer_;

    int flush();

    virtual int overflow(int c) override;
    virtual int sync() override;

private:
    out_file_buffer(const out_file_buffer&);
    out_file_buffer &operator=(const out_file_buffer&);

    static const size_t UINT32_SIZE = sizeof(uint32_t);
};


class in_file_buffer : public streambuf {
public:
    explicit in_file_buffer(fs_t* fs, const file_t& file, bool collect = false);

    vector<uint32_t> blocks();

protected:
    fs_t* fs_;
    uint32_t current_block_;

    int buffer_size_;
    char* buffer_;

    long int read_;
    long int size_;
    bool collect_;

    vector<uint32_t> blocks_;

    virtual int underflow() override;

private:
    in_file_buffer(const in_file_buffer&);
    in_file_buffer &operator=(const in_file_buffer&);

    static const size_t UINT32_SIZE = sizeof(uint32_t);
};
