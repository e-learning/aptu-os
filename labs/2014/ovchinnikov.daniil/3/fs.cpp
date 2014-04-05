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

Config get_config(const char * root){
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

FS::FS(const char *root) : root(root), config(get_config(root)), BLOCK_DATA_SIZE(config.block_size - sizeof(BlockDescriptor)) {


    initialized = ifstream(get_block_f(0), ios::binary);

    if (initialized) {
        block_map = read_block_map();
    }
}



FS::~FS() {
    write_block_map(block_map);
}

int FS::init() {
    for (int i = 0; i < config.block_number; ++i) {
        const char *block_name = get_block_f (i);
        ofstream(block_name, ios::trunc).close();
        if (truncate(block_name, config.block_size)) {
            throw "Cannot create blocks";
        }
    }
    return 0;
}

int FS::format() {

    check_initialized();

    int rd_size = sizeof(FileDescriptor);               // size of data

    {   // write root descriptor to first block
        FileDescriptor rd = create_descriptor("/");     // create root descriptor
        rd.id = 0;

        ofstream root_block(get_block_f(0), ios::binary|ios::trunc);
        root_block.write((char *)&rd, rd_size);
        root_block.close();
        if (!root_block) {
            throw "Can't write root block";
        }
    }

    vector<bool> block_map;                             // create block bitmap
    block_map.assign(config.block_number, false);       // all blocks are free
    int block_map_blocks = ceil((config.block_number / 8.0 + rd_size) / config.block_size);
    for (int i = 0; i < block_map_blocks; ++i) {        // set used blocks
        block_map[i] = true;
    }

    write_block_map(block_map);                           // write block bitmap
    return 0;
}

int FS::import(const char *source_file_path, const char *destination) {

    check_initialized();

    ifstream source_file(source_file_path, ios::binary);
    if (!source_file) {
        throw "Cannot open file to import";
    }

    FileDescriptor fd = create_file_for_write(destination);

    char * buf = new char[BLOCK_DATA_SIZE];
    size_t id = fd.first_block = find_first_free_block();
    while (source_file) {
        source_file.read(buf, BLOCK_DATA_SIZE);

        BlockDescriptor bd;
        bd.id = id;
        bd.len = source_file.gcount();
        bd.next = source_file ? find_first_free_block (id) : -1;
        block_map[bd.id] = true;

        ofstream block(get_block_f(id), ios::binary | ios::trunc);
        block.write((char *) &bd, sizeof(BlockDescriptor));
        block.write(buf, bd.len);
        block.close();
        if (!block) {
            throw "Cannot write block";
        }

        fd.number_of_blocks++;
        id = bd.next;
    }

    write_descriptor(fd);
    return 0;
}

int FS::mkdir(const char *dir) {
    create_directory_if_not_exists(split(dir, '/'));
    return 0;
}

vector<bool> FS::read_block_map() {
    vector<bool> map;
    map.assign(config.block_number, false);
    int current_block_id = 0;
    ifstream block(get_block_f(current_block_id), ios::in|ios::binary);
    block.seekg(sizeof(FileDescriptor));
    for (int i = 0; i < config.block_number; ){
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

void FS::write_block_map(const vector<bool> & map) {
    int current_block_id = 0;
    ofstream block(get_block_f(current_block_id), ios::binary|ios::app);
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
    block.close();
}
