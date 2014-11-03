#include <iostream>

#include "../fs_t.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: format <root>" << std::endl;
        return 1;
    }

    try {
        fs_t fs(argv[1], true);
        fs.format();
    } catch (fs_t::fs_error e) {
        std::cerr << "fs error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}