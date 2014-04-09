#include "block.h"

#include <fstream>

using std::ofstream;
using std::ifstream;
using std::string;

void Block::Read(std::string const& path) {
    ifstream ifs(path);
    ifs.read(buf_, size_);
}

void Block::Write(std::string const& path) {
    ofstream ofs(path.c_str());
    ofs.write(buf_, size_);
}

bool Block::SetSizeT(size_t data, size_t offset) {
    const size_t number_limit = 0xffffffff;
    if(data > number_limit || offset + SIZET_SIZE > size_) {
        return false;
    }
    for(size_t i = 0; i != SIZET_SIZE; ++i) {
        buf_[offset++] = (data % 256);
        data = data / 256;
    }
    return true;
}

bool Block::SetString(string const& data, size_t offset) {
    if(offset + data.size() > size_) {
        return false;
    }
    for(size_t i = 0; i != data.size(); ++i) {
        buf_[offset++] = data.at(i);
    }
    return true;
}

size_t Block::GetSizeT(size_t offset) {
    size_t number = 0;
    size_t current_byte = 0;
    while(current_byte != SIZET_SIZE) {
        int byte = (unsigned char)buf_[offset++];
        size_t shift = 8 * current_byte;
        number += (size_t)byte << shift;
        ++current_byte;
    }
    return number;
}

string Block::GetString(size_t offset, size_t str_size) {
    string str;
    for(size_t i = 0; i != str_size; ++i) {
        str.push_back(buf_[offset++]);
    }
    return str;
}

bool Block::SetBytes(char *bytes, size_t num, size_t offset) {
    if(offset + num > size_) {
        return false;
    }
    for(size_t i = 0; i != num; ++i) {
        buf_[offset++] = bytes[i];
    }
    return true;
}

char* Block::GetBytes(size_t num, size_t offset) {
    char* bytes = new char[num];
    for(size_t i = 0; i != num; ++i) {
        bytes[i] = buf_[offset++];
    }
    return bytes;
}
