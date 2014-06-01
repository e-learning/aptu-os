#include <iostream>
#include "filesystem.h"
#include <stdexcept>


int main (const int argc, const char *argv[]) {

    if (argc < 3) {
        std::cout << "Usage: rm root path" << std::endl;
        return 1;
    }
    try {
        FileSystem fs(argv[1]);
        fs.rm_cmd(argv[2]);

    } catch(std::runtime_error const & e) {
        std::cerr << e.what() <<std::endl;
        return 1;
    }


    return 0;
}


