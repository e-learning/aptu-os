
#include <sstream>
#include "filesystem.h"

using std::stringstream;

Block_pack::Block_pack(size_t s, const string& p) : blockSize(s), path(p) {
    if(path[path.size()-1] != '/') path.append("/");
}


//return pointer to block with specified number
Block*  Block_pack::getBlock(size_t n) {
    map<size_t, Block>::iterator i = blocks.find(n);
    if(i == blocks.end()) {
        std::stringstream s; s << path << n;
        std::pair<map<size_t, Block>::iterator, bool> pb =
                blocks.insert(std::pair<int, Block>(n, Block(n, blockSize, s.str().c_str())));
        return& (pb.first->second);
    } else {
        return& (i->second);
    }
    return 0;
}

//put data in fs
void Block_pack::save() {
    for(map<size_t, Block>::iterator i = blocks.begin(); i != blocks.end(); ++i) {
        stringstream s; s << path << i->first;
        i->second.save(s.str().c_str());
    }
}
