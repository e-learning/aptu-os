#include <iostream>
#include <cassert>
#include <ctime>
#include <algorithm>
#include "myfs.h"

int _main(int, char const * argv[]) {
    MyFileSystem fs(argv[1]);
    
    auto path = split_path(argv[2]);
    
    File file = fs.go_through_path(path);

    tm * tstmp = localtime( & file.last_modification_time );
    
    cout<< "Name Size timestamp" << endl;

    cout<< file.name << " " << file.size << " ";
    cout<< (tstmp->tm_year + 1900) << '-' 
           << (tstmp->tm_mon + 1) << '-'
           <<  tstmp->tm_mday
           << " " << tstmp->tm_hour << ":" << tstmp->tm_min << ":" << tstmp->tm_sec << endl ;
    
    if (file.is_dir) {
        FileList list(file);
        for_each(list.begin(), list.end(), [](const File & f) {
            cout<< f.name << endl;
        });
    }

    return 0;
}
