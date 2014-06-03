#include "FS/filesystem.hpp"

int main(int argc, char *argv[])
{
    if(argc != 4) {
        std::cout << "usage: export <root path> <fs_file> <host_file>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    
    fs.load();

    if(fs.exportFile(argv[2], argv[3]) == 0) {
        std::cout << "error export" << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
