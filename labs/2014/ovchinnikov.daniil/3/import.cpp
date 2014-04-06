#include <iostream>
#include "fs.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 4) {
        std::cout << "Usage: import root host_file fs_file" << std::endl;
        return 0;
    } else {
        FS fs(argv[1]);

        fs.check_initialized();

        ifstream source_file(argv[2], ios::binary);
        if (!source_file) {
            throw "Cannot open file to import";
        }

        FileDescriptor fd = fs.find_descriptor(argv[3]);
        if (fd.directory) {
            throw (string("'") + fd.name + "' is a directory").c_str();
        }

        fs.write_data(fd,source_file);
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
