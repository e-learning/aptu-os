/*
 * structs.h
 *
 *  Created on: May 7, 2014
 *      Author: atamas
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

#define MAX_NAME_LENGTH 11
#include <time.h>
#include <string>
#include <string.h>
#include <stdexcept>

struct Meta{
    int root_block;
    int block_map_size;
    char * block_map;
};

struct BlockDescriptor {
    int next = -1;
    int data_written = 0;
};


class Block{
public:
    BlockDescriptor _descriptor;
    Block(int index, int size);
    Block(int i, int size, std::string root);
    int write(void * data, int size);
    int read(void * data, int size);
    void move_to_begin(){	_descriptor.data_written = 0; }
    void reopen(){ _pos = 0; }
    char * get_data() {return _data;}
    const int get_size() const {return _size;}
    const int get_index() const {return _index;}
    void save_block(std::string root);
    bool full();
    bool end(){ return _pos == _descriptor.data_written;}
    ~Block();

private:
    void load_block();
    int _size;
    int _pos = 0;
    int _index;
    std::string _root;
    char * _data;
};

struct Config{
	int block_size;
	int block_no;
};

struct FileDescriptor {
    int id = -1;
    bool directory = true;             // if this is a directory
    int first_child = -1;            // id of block with first child file descriptor
    int parent_file = -1;            // id of block with parent file descriptor
    int next_file = -1;              // id of block with next file descriptor
    int prev_file = -1;              // id of block with next file descriptor
    int size = 0;
    int first_block = -1;            // first data block
    char filename[MAX_NAME_LENGTH];              // filename
    time_t last_update = time(0);                // last updated time
    void set_filename(std::string name){
    	for(int i = 0; i < MAX_NAME_LENGTH; ++i){
    		filename[i] = 0;
    	}
        strncpy(filename, name.c_str(), MAX_NAME_LENGTH - 1);
    }
};

#endif /* STRUCTS_H_ */
