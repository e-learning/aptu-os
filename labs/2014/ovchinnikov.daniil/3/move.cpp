#include <iostream>
#include "fs.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 4) {
        std::cout << "Usage: move root source destination" << std::endl;
    } else {
        FS fs = FS(argv[1]);

        FileDescriptor source_d = fs.find_descriptor(argv[2], false);
        FileDescriptor destination_d = fs.find_descriptor(argv[3], true, source_d.directory);

        fs.move(source_d, destination_d);
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
