#include <iostream>
#include <exception>
#include "fs.h"

int main (const int argc, const char *argv[]) try {
    if (argc < 2) {
        std::cout << "Usage: format root" << std::endl;
        return 0;
    } else {
        FS(argv[1]).format();
    }
    return 0;
}  catch (std::exception & e) {
    std::cerr << e.what() << std::endl;
    return 1;
}

