#include <iostream>

#include "../fs_t.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "usage: ls <root> <path>" << std::endl;
        return 1;
    }

    try {
        fs_t fs(argv[1]);

        std::cout << fs.ls(argv[2]) << std::endl;
    } catch (fs_t::fs_error e) {
        std::cerr << "fs error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}