#ifndef FILEHEADBLOCK_H
#define FILEHEADBLOCK_H

#include "headblock.h"
#include <fstream>

class FileHeadBlock : public HeadBlock {
public:
    FileHeadBlock(size_t s)
        : HeadBlock(s, MAGIC)
    {}

    static bool IsFileHead(std::string const& root, size_t block_ptr) {
        std::ifstream ifs(Utils::ConstructPath(root, block_ptr));
        return MAGIC == (char)ifs.get();
    }
private:
    const static char MAGIC = 'f';
};

#endif // FILEHEADBLOCK_H
