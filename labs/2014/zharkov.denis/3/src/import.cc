#include <iostream>
#include "myfs.h"

int _main(int, char const * argv[]) {

    MyFileSystem fs(argv[1]);
    
    ifstream src(argv[2], ios_base::binary);
    
    auto path = split_path(argv[3]);
    string filename = path.back();
    path.pop_back();

    File dir = fs.go_through_path(path, true);
    FileList list(dir);

    if (list.exists(filename)) {
        throw FsException(filename + " already exists");
    }

    src.seekg(0, ifstream::end);
    size_t size = src.tellg();
    src.seekg(0, ifstream::beg);
    
    fs.check_if_space_available(size);
    
    File dst = fs.new_file(filename, false, size);
    
    BlockList blocks(dst.block_id);
    auto blocks_iter = blocks.begin();

    while (size > 0) {
        block_id_t next_block_id = fs.acquire_free_block();
        BlockPtr next_block = fs.load_block(next_block_id);
        
        size_t chunk_size = min(size, fs.block_size());
        src.read(next_block->data_, chunk_size);

        fs.write_block(*next_block);
        blocks.insert_after(blocks_iter, next_block_id);
        size -= chunk_size; 
    }

    list.insert(dst);

    return 0;
}

