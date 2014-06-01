#include <iostream>
#include <algorithm>
#include "myfs.h"

MyFileSystem * fs;

void dfs(FileList list, string prefix = "/", int depth=0) {
    for (auto it = list.begin(); it != list.end(); ++it) {
        File & file = *it;
        cout<< prefix + file.name << " " << file.block_id << " " << file.size << " " << file.is_dir << " " << file.last_modification_time << " " << endl;

        if (file.is_dir) {
            dfs(FileList(file.block_id), prefix + file.name + "/", depth+1);
        }
    }
}

int _main(int, char const * argv[]) {
    fs = new MyFileSystem(argv[1]);
    
    dfs(fs->root());
    
    BlockList free(free_blocks_first);

    int cnt = 0;
    for_each(free.begin(), free.end(), [&cnt](block_id_t) {
        cnt++;
    });

    cout<< "free blocks: " << cnt << endl;
    cout<< "free size: " << cnt * fs->block_size() << endl;
    cout<< "total size: " << fs->blocks_count() * fs->block_size() << endl;


    return 0;
}
