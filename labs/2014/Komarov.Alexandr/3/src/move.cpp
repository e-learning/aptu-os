#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 4) {
        std::cout << "usage: move <root path> <src> <dst>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(fs.move(argv[2], argv[3]) == 0) {
        std::cout << "error move" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
