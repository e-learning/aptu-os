#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "usage: format <root path>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);

    if(fs.format() == 0) {
        std::cout << "error format" << std::endl;
        return 1;
    }

    return 0;
}
