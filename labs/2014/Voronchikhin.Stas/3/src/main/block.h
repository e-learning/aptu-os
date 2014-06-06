#ifndef BLOCK_H
#define BLOCK_H



class Block {
public:
    Block(size_t index, size_t size);
    Block(size_t index, size_t size, const char* data);

    void read(const char* data);
    void save(const char* data);

    char* get_data() { return blockData; }
    size_t get_size() const { return blockSize; }
    size_t get_index() const { return blockIndex; }
    Block_header* get_header() { return& blockHeader; }

private:
    Block_header blockHeader;
    size_t blockIndex;
    size_t blockSize;
    char* blockData;
};


#endif // BLOCK_H
