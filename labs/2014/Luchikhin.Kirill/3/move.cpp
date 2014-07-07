#include "filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 4) {
        std::cout << "Usage: move <root path> <src> <dst>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(!fs.move(argv[2], argv[3])) {
        std::cout << "ERROR: Unable to move file in filesystem" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
