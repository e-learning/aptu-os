#include "file_buffer.h"

out_file_buffer::out_file_buffer(fs_t* fs)
        : fs_(fs)
        , current_block_(fs->next_free_block())
        , buffer_size_(fs->block_size() - UINT32_SIZE)
        , buffer_(new char[buffer_size_]) {
    setp(buffer_, buffer_ + (buffer_size_ - 1));
}

out_file_buffer::~out_file_buffer() {
    sync();
    delete[] buffer_;
}

int out_file_buffer::sync() {
    return flush() == EOF ? -1 : 0;
}

int out_file_buffer::flush() {
    int cur_buf_size = pptr() - pbase();
    if (cur_buf_size == 0) {
        return 0;
    }

    if (current_block_ == fs_t::NO_FREE_BLOCKS) {
        return EOF;
    }

    ofstream out(fs_->block_path(current_block_), ios::binary | ios::trunc);
    if (!out.is_open()) {
        return EOF;
    }

    fs_->mark_block(current_block_, fs_t::USED);
    current_block_ = fs_->next_free_block();

    char next_block[UINT32_SIZE];
    for (size_t i = 0; i < UINT32_SIZE; i++) {
        int offset = 8 * (UINT32_SIZE - i - 1);

        next_block[i] = (current_block_ >> offset) & 0xFF;
    }

    out.write(buffer_, cur_buf_size);
    if (cur_buf_size >= buffer_size_ -1) {
        out.write(next_block, (long int) UINT32_SIZE);
    }

    pbump(-cur_buf_size);

    return cur_buf_size;
}

int out_file_buffer::overflow(int c) {
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }

    return flush() == EOF ? EOF : c;
}

in_file_buffer::in_file_buffer(fs_t* fs, const file_t &file, bool collect)
        : fs_(fs)
        , current_block_(file.start_block())
        , buffer_size_(max(file.size(), fs->block_size() - UINT32_SIZE))
        , buffer_(new char[buffer_size_])
        , read_(0)
        , size_(file.size())
        , collect_(collect) {
    setg(buffer_, buffer_, buffer_);
    if (collect_) {
        blocks_.push_back(current_block_);
    }
}

vector<uint32_t> in_file_buffer::blocks() {
    return blocks_;
}

int in_file_buffer::underflow() {
    if (gptr() < egptr()) {
        return *gptr();
    }

    if (read_ >= size_) {
        return EOF;
    }

    ifstream in(fs_->block_path(current_block_), ios::binary);
    if (!in.is_open()) {
        return EOF;
    }

    long int bytes_remaining = size_ - read_;
    if (bytes_remaining > (long int) fs_->block_size()) {
        bytes_remaining = fs_->block_size() - UINT32_SIZE;
        in.read(buffer_, bytes_remaining);
        bytes_remaining = in.gcount();

        unsigned char next_block[UINT32_SIZE];
        in.read((char*) next_block, UINT32_SIZE);
        current_block_ = 0;
        for (size_t i = 0; i < UINT32_SIZE; i++) {
            uint32_t n = 0;
            n = next_block[i];
            int offset = 8 * (UINT32_SIZE - i - 1);
            n = n << offset;
            current_block_ |= n;
        }

        if (collect_) {
            blocks_.push_back(current_block_);
        }
    } else {
        in.read(buffer_, bytes_remaining);
        bytes_remaining = in.gcount();
        current_block_ = fs_t::NO_FREE_BLOCKS;
    }

    read_ += bytes_remaining;
    setg(buffer_, buffer_, buffer_ + bytes_remaining);

    return traits_type::to_int_type(*gptr());
}
