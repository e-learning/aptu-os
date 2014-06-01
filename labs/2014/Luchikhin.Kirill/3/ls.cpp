#include "filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "Usage: ls <root path> <dir>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(!fs.fileInfo(argv[2])) {
        std::cout << "ERROR: Unable to get file info from filesystem" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
