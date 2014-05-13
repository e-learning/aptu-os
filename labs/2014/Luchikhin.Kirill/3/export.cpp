#include "filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 4) {
        std::cout << "Usage: export <root path> <fs_file> <host_file>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(!fs.exportFile(argv[2], argv[3])) {
        std::cout << "ERROR: Unable to export file from filesystem" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
