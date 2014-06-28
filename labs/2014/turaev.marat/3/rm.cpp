#include <iostream>
#include "file_system.h"

using namespace std;

int main(const int argc, const char *argv[]) try {
    if (argc < 3) {
        std::cout << "Usage: rm root path" << std::endl;
        return 0;
    } else {
        file_system fs = file_system(argv[1]);
        file_descriptor d = fs.find_descriptor(argv[2], false);
        fs.rm(d);
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
} catch (const string msg) {
    std::cerr << msg << std::endl;
    return 1;
}

