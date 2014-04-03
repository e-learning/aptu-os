#include <iostream>
#include <cassert>
#include "myfs.h"

void rec_copy(MyFileSystem & fs, File const & src, File const & dst) {
    if (src.is_dir && !dst.is_dir) {
        throw FsException(string(src.name) + " is not dir");
    }
    
    if (!src.is_dir) {
        assert(!dst.is_dir);
        
        fs.check_if_space_available(src.size);

        BlockList src_blocks(src.block_id);
        BlockList dst_blocks(dst.block_id);
        dst_blocks.release_all_blocks();
        auto blocks_iter = dst_blocks.begin();

        for_each(src_blocks.begin(), src_blocks.end(), [&fs, &dst_blocks, &blocks_iter](block_id_t id) {
            auto src_block = fs.load_block(id);
            block_id_t dst_block_id = fs.acquire_free_block();

            dst_blocks.insert_after(blocks_iter, dst_block_id);
            
            auto dst_block = fs.load_block(dst_block_id);
            dst_block->copy_from(*src_block);
            fs.write_block(*dst_block);
        });
        
        return;
    }

    FileList src_dir(src);
    FileList dst_dir(dst);
    
    for_each(src_dir.begin(), src_dir.end(), [&fs, &dst_dir](File const & f){
        auto file_in_dst = dst_dir.find(f.name);
        File dst_f;

        if (file_in_dst == dst_dir.end()) {
            dst_f = fs.new_file(f.name, f.is_dir, f.size);
            dst_dir.insert(dst_f);
        } else {
            dst_f = *file_in_dst;

            if (f.is_dir != dst_f.is_dir) {
                throw FsException("Wrong type of files: " + f.name);
            }
        }

        rec_copy(fs, f, dst_f);
    });
}

int _main(int, char const * argv[]) {
    MyFileSystem fs(argv[1]);
    
    auto src_path = split_path(argv[2]);
    auto dst_path = split_basename(argv[3]);

    File src = fs.go_through_path(src_path);
    File dst_dir = fs.go_through_path(dst_path.first);
    FileList dst_dir_list(dst_dir);
    
    File dst;

    if (dst_dir_list.exists(dst_path.second)) {
        dst = *(dst_dir_list.find(dst_path.second));
        
        if (dst.is_dir) {
            File dst_new = fs.new_file(src.name, src.is_dir, src.size);
            FileList(dst).insert(dst_new);
            dst = dst_new;
        }
    } else {
        dst = fs.new_file(dst_path.second, src.is_dir, src.size);
        dst_dir_list.insert(dst);
    }
    
    rec_copy(fs, src, dst);

    return 0;
}

