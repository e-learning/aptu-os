#include <iostream>
#include <cassert>
#include "myfs.h"

int _main(int, char const * argv[]) {
    MyFileSystem fs(argv[1]);
    
    auto path = split_path(argv[2]);
    
    File curdir = fs.root_directory();

    for (string const & part : path) {
        FileList list(curdir);

        auto next = list.find(part);
        if (next != list.end()) {
            if (!next->is_dir) {
                throw FsException(part + "is not dir");
            }
            curdir = *next;
        } else {
            curdir = fs.new_file(part, true);
            list.insert(curdir);
        }
    }

    return 0;
}

