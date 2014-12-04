#include "util.h"
#include <algorithm>
#include <fstream>
#include <iostream>

block::block(int index, int size) :
        _index(index)
{
    _size = size - sizeof(block_descriptor);
    _data = new char[_size];
}

block::block(int index, int size, std::string root) :
        _index(index)
{
    _size = size - sizeof(block_descriptor);
    _data = new char[_size];
    load_block(root);
}

void block::load_block(std::string root)
{
    std::string path = root + "/" + std::to_string(_index);
    std::ifstream block_file(path, std::ios::in | std::ios::binary);
    if (block_file.fail()) throw std::runtime_error("Can't read block №" + std::to_string(_index));
    block_file.read((char *) &_descriptor, sizeof(block_descriptor));
    block_file.read(_data, _size);
    if (block_file.fail()) throw std::runtime_error("Error while reading block №" + std::to_string(_index));
    block_file.close();
}

int block::write(void *data, int size)
{
    int size_to_write = std::min(size, _size - _descriptor.data_written);
    memcpy(_data + _descriptor.data_written, data, size_to_write);
    _descriptor.data_written += size_to_write;
    return size_to_write;
}

int block::read(void *data, int size)
{
    int size_to_read = std::min(size, _descriptor.data_written - _pos);
    memcpy(data, _data + _pos, size_to_read);
    _pos += size_to_read;
    return size_to_read;
}

bool block::full()
{
    return _descriptor.data_written == _size;
}

void block::save_block(std::string root)
{
    std::string path = root + "/" + std::to_string(_index);
    std::ofstream block_file(path, std::ios::out | std::ios::binary);
    block_file.write((char *) &_descriptor, sizeof(_descriptor));
    block_file.write(_data, _size);
    block_file.close();
}

block::~block()
{

}
