#include <iostream>
#include "fs.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 3) {
        std::cout << "Usage: rm root path" << std::endl;
        return 0;
    } else {
        FS f = FS(argv[1]);
        FileDescriptor d = f.find(argv[2]);
        f.rm(d);
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
