#include "../main/filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "wrong arg number! Usage: rm <root path> <file in fs>" << std::endl;
        return 1;
    }

    File_system fs(argv[1]);
    
    fs.load();

    if(fs.remove(argv[2]) == 0) {
        std::cout << "failed on remove"<< argv[2]<< std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
