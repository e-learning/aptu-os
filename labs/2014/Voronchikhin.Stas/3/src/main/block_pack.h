#ifndef BLOCK_PACK_H
#define BLOCK_PACK_H

#include <string>
#include <map>


using std::string;
using std::map;

class Block_pack {
public:
    Block_pack(size_t size, const string& path);

    Block* getBlock(size_t n);
    void save();

private:
    size_t blockSize;
    std::string path;
    map<size_t, Block> blocks;
};

#endif // BLOCK_PACK_H
