#include <iostream>
#include "filesystem.h"
#include <stdexcept>


int main (const int argc, const char *argv[]) {

    if (argc < 4) {
        std::cout << "Usage: move root source target" << std::endl;
        return 1;
    }
    try {
        FileSystem fs(argv[1]);
        fs.move_cmd(argv[2], argv[3]);

    } catch(std::runtime_error const & e) {
        std::cerr << e.what() <<std::endl;
        return 1;
    }


    return 0;
}




