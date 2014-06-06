#include "filesystem.h"

Block::Block(size_t index, size_t size) {
    blockIndex = index;
    blockSize = size - sizeof(Block_header);
    blockData = new char[blockSize];
}

Block::Block(size_t i, size_t s, const char*  block_name) {
    blockIndex = i;
    blockSize = s - sizeof(Block_header);
    blockData = new char[blockSize];
    read(block_name);
}


void Block::read(const char*  block_name) {
    FILE*  blockFile = fopen(block_name, "rb");
    fread(&blockHeader, sizeof(Block_header), 1, blockFile);
    fread(blockData, sizeof(char), blockSize, blockFile);
    fclose(blockFile);
}

void Block::save(const char*  block_name) {
    FILE*  file_block = fopen(block_name, "wb");
    fwrite(&blockHeader, sizeof(Block_header), 1, file_block);
    fwrite(blockData, sizeof(char), blockSize, file_block);
    fclose(file_block);
}
