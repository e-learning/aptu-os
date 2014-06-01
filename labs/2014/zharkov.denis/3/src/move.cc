#include <iostream>
#include <cassert>
#include "myfs.h"

int _main(int, char const * argv[]) {
    MyFileSystem fs(argv[1]);
    
    auto src_path = split_basename(argv[2]);
    auto dst_path = split_basename(argv[3]);

    File src_dir = fs.go_through_path(src_path.first);
    File dst_dir = fs.go_through_path(dst_path.first);
    FileList src_dir_list(src_dir);
    FileList dst_dir_list(dst_dir);
    
    auto src_pos = src_dir_list.find(src_path.second);
    
    if (src_pos == src_dir_list.end()) {
        throw FsException(string(argv[2]) + " doesn't exist");
    }
    
    File src = *src_pos;

    src_dir_list.remove(src_pos);

    File dst;
    
    src.name = dst_path.second;
    
    if (dst_dir_list.exists(dst_path.second)) {
        auto dst_pos = (dst_dir_list.find(dst_path.second));
        dst = *dst_pos;

        if (!dst.is_dir) {
            BlockList(dst.block_id).release_all_blocks();
            dst_dir_list.remove(dst_pos);
        } else {
            dst_dir_list = FileList(dst.block_id);
            src.name = src_path.second;
        }
    } 
    
    dst_dir_list.insert(src);
        
    return 0;
}
