#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "usage: ls <root path> <dir>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(fs.fileInfo(argv[2]) == 0) {
        std::cout << "error ls" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
