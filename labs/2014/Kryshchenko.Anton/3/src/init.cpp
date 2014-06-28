#include <iostream>
#include "filesystem.h"
#include <stdexcept>

using std::string;

int main (const int argc, const char *argv[]) {

    if (argc < 2) {
        std::cout << "Usage: init root" << std::endl;
        return 1;
    }
    try {
        FileSystem fs(argv[1]);
        fs.init_cmd();

    } catch(std::runtime_error const & e) {
        std::cerr << e.what() <<std::endl;
        return 1;
    }


    return 0;
}
