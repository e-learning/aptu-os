//============================================================================
// Name        : allocator.cpp
// Author      : atamas
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

unsigned char * mem;

int mem_size;
struct mem_block;

mem_block * get_block(int offset){
	return reinterpret_cast<mem_block *>(mem+offset);
}

struct mem_block{
	unsigned short _offset;
	unsigned short _prev;
	unsigned short _size;
	bool _free;

	mem_block * get_next(){
		if( _offset + _size < mem_size){
			return get_block( _offset + _size );
		} else{
			return 0;
		}
	}

	bool has_next(){
		if( _offset + _size < mem_size){
			return 1;
		} else{
			return 0;
		}
	}

	bool has_prev(){
		if (_offset != 0){
			return 1;
		} else{
			return 0;
		}
	}

	mem_block * get_prev(){
		if (_offset != 0){
			return get_block(_prev);
		} else{
			return 0;
		}
	}

	int usr_mem_size(){
		return _size - sizeof(mem_block);
	}

	int usr_mem_offset(){
		return _offset + sizeof(mem_block);
	}
};

int alloc(int size){
	for(mem_block * i = get_block(0); i; i = i->get_next() ){
		if( i->_free && i->usr_mem_size() >= size ){
			if( i->usr_mem_size() - size <  sizeof(mem_block) ){
				i->_free = false;
				return i->_offset + sizeof(mem_block);
			} else{
				mem_block * next = get_block( i->usr_mem_offset() + size );
				next->_prev = i->_offset;
				next->_offset = i->usr_mem_offset() + size;
				next->_size = i->_size - (next->_offset - i->_offset);
				next->_free = true;
				i->_free = false;
				i->_size = next->_offset - i->_offset;
				return i->_offset + sizeof(mem_block);
			}
		}
	}
	return -1;
}

void free(int offset){
	offset -= sizeof(mem_block);
	mem_block * block = get_block(offset);
	block->_free = true;
	if( block->has_prev() && block->has_next() && block->get_prev()->_free && block->get_next()->_free ){
		if(block->get_next()->has_next()){
			block->get_next()->get_next()->_prev = block->get_prev()->_offset;
		}
		block->get_prev()->_size += ( block->_size + block->get_next()->_size);
	} else if( block->has_prev() && block->get_prev()->_free ){
		if(block->has_next()){
			block->get_next()->_prev = block->get_prev()->_offset;
		}
		block->get_prev()->_size += block->_size;
	} else if( block->has_next() && block->get_next()->_free ){
		if(block->get_next()->has_next()){
			block->get_next()->get_next()->_prev = block->_offset;
		}
		block->_size += block->get_next()->_size;
		block->_free = true;
	}
}

void info(){
	int usr_blocks = 0;
	int usr_mem = 0;
	int max_size = 0;

	for( mem_block * i = get_block(0); i ; i = i->get_next() ){
		if(i->_free){
			max_size = std::max( max_size, i->usr_mem_size() );
		} else{
			++usr_blocks;
			usr_mem += i->usr_mem_size();
		}
	}

	std::cout << usr_blocks << std::endl;
	std::cout << usr_mem << std::endl;
	std::cout << max_size << std::endl;
}

void mmap(){
	for( mem_block * i = get_block(0); i ; i = i->get_next() ){
		for(unsigned int j = 0; j < sizeof(mem_block); ++j){
			std::cout << 'm';
		}

		if(i->_free){
			for(int j = 0; j < i->usr_mem_size(); ++j){
				std::cout << 'f';
			}
		} else{
			for(int j = 0; j < i->usr_mem_size(); ++j){
				std::cout << 'u';
			}
		}
	}
	std::cout << std::endl;
}

int main() {

	cin >> mem_size;
	mem = new unsigned char[mem_size];
	mem_block * memory = get_block(0);
	memory->_size = mem_size;
	memory->_offset = 0;
	memory->_free = true;

	while(true){
		std::string command;
		std::cin >> command;
		if(command == "ALLOC"){
			int size;
			std::cin >> size;
			int pos = alloc(size);
			if(pos == -1){
				std::cout << '-' << std::endl;
			} else{
				std::cout << '+' << pos << std::endl;
			}
		} else if(command == "FREE"){
			int offset;
			std::cin >> offset;
			free(offset);
		} else if(command == "INFO"){
			info();
		} else if(command == "MAP"){
			mmap();
		}
	}
	return 0;
}
