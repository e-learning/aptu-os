#include <iostream>
#include "fs.h"

int main (const int argc, const char *argv[]) try {
    if (argc < 2) {
        std::cout << "Usage: format root" << std::endl;
        return 0;
    } else {
        FS(argv[1]).format();
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
} catch (const string msg) {
    std::cerr << msg << std::endl;
    return 1;
}
