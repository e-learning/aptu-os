#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 4) {
        std::cout << "usage: copy <root path> <src> <dst>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    fs.load();

    if(fs.copy(argv[2], argv[3]) == 0) {
        std::cout << "error copy" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}

