#include "../main/filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "wrong arg number! Usage: format <root path>" << std::endl;
        return 1;
    }

    File_system fs(argv[1]);

    if(fs.format() == 0) {
        std::cout << "failed format" << std::endl;
        return 1;
    }

    return 0;
}
