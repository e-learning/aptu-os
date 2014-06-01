#include <iostream>
#include "myfs.h"

int _main(int, char const * argv[]);

int main(int c, char const * argv[]) {
    try {
        return _main(c, argv);
    } catch(FsException & e) {
        cerr<< e.what() << endl;
        return 1;
    }
}
