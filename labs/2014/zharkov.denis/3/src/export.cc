#include <iostream>
#include <cassert>
#include "myfs.h"

int _main(int, char const * argv[]) {
    MyFileSystem fs(argv[1]);
    
    auto path = split_path(argv[2]);
    
    File src = fs.go_through_path(path);

    BlockList blocks(src.block_id);
    ofstream dst(argv[3], ios_base::binary);
    
    size_t size = src.size;
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        BlockPtr next_block = fs.load_block(*it);
        
        size_t chunk_size = min(size, fs.block_size());
        dst.write(next_block->data_, chunk_size);
            
        size -= chunk_size; 
    }

    assert(size == 0);

    return 0;
}

