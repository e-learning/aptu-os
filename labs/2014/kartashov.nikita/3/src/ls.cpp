#include <iostream>
#include "utils/file_system.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 3) {
        std::cout << "Usage: ls root path" << std::endl;
        return 0;
    } else {
        file_system fs(argv[1]);
        file_record fd = fs.find_descriptor(argv[2], false);
        if (fd.directory) {
            cout << fs.list(fd);
        } else {
            cout << fd;
        }
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
} catch (const string msg) {
    std::cerr << msg << std::endl;
    return 1;
}

