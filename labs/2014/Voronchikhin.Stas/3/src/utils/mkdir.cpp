#include "../main/filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cout << "wrong arg number! Usage: mkdir <root path> <path>" << std::endl;
        return 1;
    }

    File_system fs(argv[1]);
    
    fs.load();

    if(fs.forced_create_dir(argv[2]) == 0) {
        std::cout << "failed mkdir "<< argv[2] << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
