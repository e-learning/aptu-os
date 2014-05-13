#include <iostream>
#include <exception>
#include "fs.h"

int main (const int argc, const char *argv[]) try {
    if (argc < 2) {
        std::cout << "Usage: init root" << std::endl;
    } else {
        FS fs(argv[1]);
        fs.init();
    }
    return 0;
} catch (std::exception & e) {
    std::cerr << e.what() << std::endl;
    return 1;
}
