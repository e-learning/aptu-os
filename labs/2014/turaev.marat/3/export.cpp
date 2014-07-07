#include <iostream>
#include "file_system.h"

using namespace std;

int main(const int argc, const char *argv[]) try {
    if (argc < 4) {
        std::cout << "Usage: export root fs_path host_path" << std::endl;
    } else {
        file_system fs = file_system(argv[1]);
        file_descriptor fd = fs.find_descriptor(argv[2], false);
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
} catch (const string msg) {
    std::cerr << msg << std::endl;
    return 1;
}
