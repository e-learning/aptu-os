#include <iostream>

#include "../fs_t.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "usage: move <root> <src> <dst>" << std::endl;
        return 1;
    }

    try {
        fs_t fs(argv[1]);
        fs.move(argv[2], argv[3]);
    } catch (fs_t::fs_error e) {
        std::cerr << "fs error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}