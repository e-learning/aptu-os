#include <iostream>
#include "fs.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 3) {
        std::cout << "Usage: mkdir root path" << std::endl;
        return 0;
    } else {
        if (!FS(argv[1]).find_descriptor(argv[2], true, true).directory) {
            throw (string(argv[2]) + " is a file").c_str();
        }
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
