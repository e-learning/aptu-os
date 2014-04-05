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

int FS::init() {
    for (int i = 0; i < config.block_number; ++i) {
        const char *block_name = get_block_f (i);
        ofstream(block_name).close();
        if (truncate(block_name, config.block_size)) {
            throw "Cannot create blocks";
        }
    }
    return 0;
}

int FS::format() {

    FileDescriptor rd;                                  // create root descriptor
    rd.directory        = true;
    rd.first_file       = -1;
    rd.next_file        = -1;
    rd.first_block      = -1;
    rd.number_of_blocks = 0;
    strncpy(rd.name, "/", sizeof(rd.name));
    rd.time             = time(0);

    int rd_size = sizeof(FileDescriptor);               // size of data

    if (!ifstream(get_block_f (0))) {                   // check ig root block exists
        throw "File system isn't initialized yet";
    }
    ofstream root_block(get_block_f(0), ios::binary|ios::trunc);    // write root descriptor to first block
    root_block.write((char *)&rd, rd_size);
    root_block.flush();
    root_block.close();
    if (!root_block) {
        throw "Can't write root block";
    }

    vector<bool> block_map;                             // create block bitmap
    block_map.assign(config.block_number, false);       // all blocks are free
    int block_map_blocks = ceil((config.block_number / 8.0 + rd_size) / config.block_size);
    for (int i = 0; i < block_map_blocks; ++i) {        // set used blocks
        block_map[i] = true;                            // theese blocks are the root block and blocks used for the bitmap
    }

    writeBlockMap(block_map);                           // write block bitmap
    return 0;
}

int FS::import(const char *filename, const char * destination) {
    vector<bool> map = read_block_map();
    vector<string> path;
    {
        std::istringstream ss(string(destination));

        string item;

        item.find('/');
        //        while (std::getline(ss, item)) {
        //            if (item != "")
        //                path.push_back(item);
        //        }
    }
    return 0;
}

vector<bool> FS::read_block_map() {
    vector<bool> map;
    map.assign(config.block_number, false);
    int current_block_id = 0;
    ifstream block(get_block_f(current_block_id), ios::in|ios::binary);
    block.seekg(sizeof(FileDescriptor));
    for (int i = 0; i< config.block_number; ){
        char byte;
        block >> byte;
        for (int b = 0; b < 8 && i < config.block_number; b++,i++) {
            map[i] = byte & (1 << b);
        }
        if (block.tellg() == config.block_size){
            block.close();
            block.open(get_block_f(++current_block_id), ios::binary);
        }
    }
    return map;
}

void FS::writeBlockMap(const vector<bool> & map) {
    int current_block_id = 0;
    ofstream block(get_block_f(current_block_id), ios::binary|ios::trunc);
    if (!block) {
        throw "Can't open block";
    }
    block.seekp(sizeof(FileDescriptor));
    for (int i = 0; i < config.block_number;) {
        char byte = 0;
        for (int b = 0; b < 8 && i < config.block_number; b++,i++) {
            byte |= map[i] << b;
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
}
