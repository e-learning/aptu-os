#ifndef __DATA_H
#define __DATA_H

#include <time.h>

struct FileDescriptor {
    bool directory;         // if this is a directory
    int first_file;         // id of block with first child file descriptor
    int next_file;          // id of block with next file descriptor
    int first_block;        // first data block
    int number_of_blocks;   // total blocks in file
    char name[10];          // filename
    time_t time;            // last updated time
};

struct BlockDescriptor {
    int next;               // next block id
    size_t len;             // size of data written to block, -1 if block if full
};

struct Config {
    int block_size;         // size of block
    int block_number;       // number of blocks
};

#endif // __DATA_H
