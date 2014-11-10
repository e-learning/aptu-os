#include "util.h"
#include <algorithm>
#include <fstream>
#include <iostream>

block_t::block_t(int index, int size) :
        _index(index)
{
    size = (int) (size - sizeof(block_desc));
    _data = new char[size];
}

block_t::block_t(int index, int size, std::string root) :
        _index(index)
{
    size = (int) (size - sizeof(block_desc));
    _data = new char[size];
    load_block(root);
}

void block_t::load_block(std::string root)
{
    std::string path = root + "/" + std::to_string(_index);
    std::ifstream block_file(path, std::ios::in | std::ios::binary);
    if (block_file.fail()) throw std::runtime_error("Can't read block №" + std::to_string(_index));
    block_file.read((char *) &_descriptor, sizeof(block_desc));
    block_file.read(_data, size);
    if (block_file.fail()) throw std::runtime_error("Error while reading block №" + std::to_string(_index));
    block_file.close();
}

int block_t::write(void *data, int size)
{
    int size_to_write = std::min(size, size - _descriptor.data_written);
    memcpy(_data + _descriptor.data_written, data, (size_t) size_to_write);
    _descriptor.data_written += size_to_write;
    return size_to_write;
}

int block_t::read(void *data, int size)
{
    int size_to_read = std::min(size, _descriptor.data_written - pos);
    memcpy(data, _data + pos, (size_t) size_to_read);
    pos += size_to_read;
    return size_to_read;
}

bool block_t::is_full()
{
    return _descriptor.data_written == size;
}

void block_t::save_block(std::string root)
{
    std::string path = root + "/" + std::to_string(_index);
    std::ofstream block_file(path, std::ios::out | std::ios::binary);
    block_file.write((char *) &_descriptor, sizeof(_descriptor));
    block_file.write(_data, size);
    block_file.close();
}

block_t::~block_t()
{
    delete[] _data;
}
