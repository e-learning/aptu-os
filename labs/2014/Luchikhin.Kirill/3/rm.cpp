#include "filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "Usage: rm <root path> <file>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(!fs.remove(argv[2])) {
        std::cout << "ERROR: Unable to remove file from filesystem" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
