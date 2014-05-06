#include "filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "Usage: mkdir <root path> <path>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(!fs.forcedCreateDir(argv[2])) {
        std::cout << "ERROR: Unable to create directory in filesystem" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
