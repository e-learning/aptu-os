#include "structs.h"
#include <algorithm>


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
    const char * path = (_root +  "/" + std::to_string(_index)).c_str();
    FILE *blockFile = fopen( path, "rb");
    if(!fread(&_descriptor, sizeof(BlockDescriptor), 1, blockFile)) throw std::runtime_error("Can't read block");
    if(!fread(_data, sizeof(char), _size, blockFile)) throw std::runtime_error("Can't read block");
    fclose(blockFile);
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
    const char * path = (root + "/" + std::to_string(_index)).c_str();
    FILE *blockFile = fopen(path, "wb");
    fwrite(&_descriptor, sizeof(BlockDescriptor), 1, blockFile);
    fwrite(_data, sizeof(char), _size, blockFile);
    fclose(blockFile);
}

