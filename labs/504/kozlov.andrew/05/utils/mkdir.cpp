#include <iostream>

#include "../fs_t.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "usage: mkdir <root> <path>" << std::endl;
        return 1;
    }

    try {
        fs_t fs(argv[1]);
        fs.mkdir(argv[2]);
    } catch (fs_t::fs_error e) {
        std::cerr << "fs error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}