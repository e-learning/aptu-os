#include "fs.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <unistd.h>

using std::vector;
using std::ofstream;
using std::ifstream;
using std::ios;
using std::string;
using std::istringstream;
using std::to_string;

FS::FS(const char *root)
    : root(root),
      root_d(read_descriptor(0)),
      config(get_config()),
      BLOCK_DATA_SIZE(config.block_size - sizeof(BlockDescriptor))  {

    initialized = ifstream(get_block_f(0), ios::binary);

    if (initialized) {
        read_block_map();
    }
}

FS::~FS() {
    if (initialized) {
        write_block_map();
    }
}

void FS::format() {
    check_initialized();

    int rd_size = sizeof(FileDescriptor);               // size of data
    root_d = create_descriptor("/");                    // recreate root descriptor
    root_d.id = 0;
    block_map.assign(config.block_number, false);       // all blocks are free
    int block_map_blocks = ceil((config.block_number / 8.0 + rd_size) / config.block_size);
    for (int i = 0; i < block_map_blocks; ++i) {        // set used blocks
        block_map[i] = true;
    }
}

void FS::rm(FileDescriptor fd) {
    if (fd.directory) {
        if (fd.first_child != -1) {
            FileDescriptor c = read_descriptor(fd.first_child);
            while (c.next_file!= -1){
                rm(c);
                c = read_descriptor(c.next_file);
            }
            rm(c);
        }
    } else {
        clear_descriptor(fd);
    }

    remove_descriptor(fd);
}

void FS::read_block_map() {
    block_map.clear();
    block_map.assign(config.block_number, false);
    int current_block_id = 0;
    ifstream block(get_block_f(current_block_id), ios::in|ios::binary);
    block.seekg(sizeof(FileDescriptor));
    for (int i = 0; i < config.block_number; ){
        char byte;
        block >> byte;
        for (int b = 0; b < 8 && i < config.block_number; b++,i++) {
            block_map[i] = byte & (1 << b);
        }
        if (block.tellg() == config.block_size){
            block.close();
            block.open(get_block_f(++current_block_id), ios::binary);
        }
    }
}

void FS::write_block_map() {
    int current_block_id = 0;
    ofstream block(get_block_f(current_block_id), ios::binary|ios::trunc);
    block.write((char *)&root_d, sizeof(FileDescriptor));
    if (!block) {
        throw "Can't open block";
    }
    block.seekp(sizeof(FileDescriptor));
    for (int i = 0; i < config.block_number;) {
        char byte = 0;
        for (int b = 0; b < 8 && i < config.block_number; b++,i++) {
            byte |= block_map[i] << b;
        }
        block << byte;
        if (!block) {
            throw "Write error during writing bitmap";
        }
        if (block.tellp() == config.block_size){
            block.close();
            block.open(get_block_f(++current_block_id), ios::binary);
            if (!block) {
                throw "Can't open block";
            }
        }
    }
    block.close();
}

Config FS::get_config(){
    Config config;
    std::ifstream config_file((string(root) + "/config").c_str());
    if (!config_file.good()) {
        throw "Cannot open config file";
    }
    config_file  >> config.block_size >> config.block_number;
    if (!config_file.good()
            || config.block_size > 50000000 || config.block_size < 1024
            || config.block_number > 50000 || config.block_number < 1) {
        throw "Config file is bad";
    }
    return config;
}
