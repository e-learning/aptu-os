#include "../main/filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 4) {
        std::cout << "wrong arg number! Usage: move <root path> <src> <target>" << std::endl;
        return 1;
    }

    File_system fs(argv[1]);
    
    fs.load();

    if(fs.move(argv[2], argv[3]) == 0) {
        std::cout << "failed on move "<<argv[1]<< " to "<<argv[3]<< std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
