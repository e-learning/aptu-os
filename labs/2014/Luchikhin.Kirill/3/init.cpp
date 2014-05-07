#include "filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "Usage: init <root path>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);
    if(!fs.init()) {
        std::cout << "ERROR: Unable to init filesystem" << std::endl;
        return 1;
    }

    return 0;
}

