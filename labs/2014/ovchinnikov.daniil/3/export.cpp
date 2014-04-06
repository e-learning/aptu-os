#include <iostream>
#include "fs.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 4) {
        std::cout << "Usage: export root fs_path host_path" << std::endl;
    } else {
        FS fs = FS(argv[1]);
        FileDescriptor fd = fs.find_descriptor(argv[2], false);

        ofstream out_file(argv[3], ios::binary | ios::trunc);
        if (!out_file) {
            throw "Cannot open destination file";
        }

        fs.read_data(fd, out_file);
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
