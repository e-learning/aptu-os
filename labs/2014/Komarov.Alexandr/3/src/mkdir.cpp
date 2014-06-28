#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "usage: mkdir <root path> <path>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(fs.forcedCreateDir(argv[2]) == 0) {
        std::cout << "error mkdir" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
