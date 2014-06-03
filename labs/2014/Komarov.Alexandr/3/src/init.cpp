#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "usage: init <root path>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    if(fs.init() == 0) {
        std::cout << "error init" << std::endl;
        return 1;
    }

    return 0;
}

