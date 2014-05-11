#include "structs.h"
#include <algorithm>
#include <fstream>
#include <iostream>

Block::Block(int index, int size):
_index(index){
    _size = size - sizeof(BlockDescriptor);
    _data = new char[_size];
}

Block::Block(int index, int size, std::string root):
_index(index),
_root(root){
    _size = size - sizeof(BlockDescriptor);
    _data = new char[_size];
    load_block();
}

void Block::load_block(){
    std::string path = _root +  "/" + std::to_string(_index);
    std::cout << path << std::endl;
    std::ifstream block_file ( path, std::ios::in | std::ios::binary);
    block_file.read((char *)&_descriptor, sizeof(BlockDescriptor));
    block_file.read(_data, _size);
    if(block_file.fail()) throw std::runtime_error("Can't read block");
    block_file.close();
}

int Block::write(void * data, int size){
    int size_to_write = std::min( size, _size - _descriptor.data_written);
    memcpy( _data + _descriptor.data_written, data, size_to_write);
    _descriptor.data_written += size_to_write;
    return size_to_write;
}

int Block::read(void * data, int size){
	int size_to_read = std::min( size, _descriptor.data_written - _pos);
    memcpy( data, _data + _pos, size_to_read);
    _pos += size_to_read;
    return size_to_read;
}

bool Block::full(){
    return _descriptor.data_written == _size;
}

void Block::save_block(std::string root){
    std::string path = root + "/" + std::to_string(_index);
    std::ofstream block_file (path, std::ios::out|std::ios::binary);
    block_file.write((char*)&_descriptor, sizeof(_descriptor));
    block_file.write(_data,_size);
    block_file.close();
}

