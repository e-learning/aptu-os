#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "usage: rm <root path> <file>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(fs.remove(argv[2]) == 0) {
        std::cout << "error rm" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
