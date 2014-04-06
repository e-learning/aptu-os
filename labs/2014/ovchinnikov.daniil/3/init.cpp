#include <iostream>
#include "fs.h"

int main (const int argc, const char *argv[]) try {
    if (argc < 2) {
        std::cout << "Usage: init root" << std::endl;
        return 0;
    } else {
        FS fs(argv[1]);
        Config config = fs.get_config();
        for (int i = 0; i < config.block_number; ++i) {
            const char *block_name = fs.get_block_f(i);
            ofstream(block_name, ios::trunc).close();
            if (truncate(block_name, config.block_size)) {
                throw "Cannot create blocks";
            }
        }
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
