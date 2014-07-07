#ifndef DIRHEADBLOCK_H
#define DIRHEADBLOCK_H

#include "headblock.h"
#include <fstream>

class DirHeadBlock : public HeadBlock {
public:
    DirHeadBlock(size_t s)
        : HeadBlock(s, MAGIC)
    {}

    static bool IsDirHead(std::string const& root, size_t block_ptr) {
        std::ifstream ifs(Utils::ConstructPath(root, block_ptr));
        return MAGIC == (char)ifs.get();
    }

private:
    const static char MAGIC = 'd';
};

#endif // DIRHEADBLOCK_H
