#ifndef STRUCTS_H
#define STRUCTS_H

struct Block_header {
    int next_block;
};

struct Directory_header {
    size_t files_in_dir;
};

struct Super_block_info {
    int block_size;
    int number_of_blocks;
    int next_block;
    int root_block;
    int bitmap_size;
    char *bitmap;
};

#endif //STRUCTS_H
