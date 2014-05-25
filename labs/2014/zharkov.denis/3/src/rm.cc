#include <iostream>
#include <cassert>
#include <algorithm>
#include "myfs.h"

void rm_rec(MyFileSystem & fs, File const & f) {
    if (!f.is_dir) {
        BlockList(f.block_id).release_all_blocks();
        fs.release_block(f.block_id);
        return;
    }

    FileList dir(f);

    for_each(dir.begin(), dir.end(), [&fs](File const & df){
        rm_rec(fs, df);
    });

    dir.clear();
}

int _main(int, char const * argv[]) {
    MyFileSystem fs(argv[1]);
    
    if (argv[2] == string("/")) {
        throw FsException("don't delete root");
    }

    auto path = split_basename(argv[2]);

    File dir = fs.go_through_path(path.first);
    FileList dir_list(dir);
    
    auto pos = dir_list.find(path.second);
    
    if (pos == dir_list.end()) {
        throw FsException(string(argv[2]) + " doesn't exist");
    }
    
    File f = *pos;

    dir_list.remove(pos);
    
    rm_rec(fs, f);
        
    return 0;
}
