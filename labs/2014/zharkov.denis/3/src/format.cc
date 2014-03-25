#include "myfs.h"

int _main(int, char const * argv[]) {
    MyFileSystem fs(argv[1]);
    fs.format();

    return 0;
}
