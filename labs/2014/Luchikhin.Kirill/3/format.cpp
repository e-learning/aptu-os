#include "filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "Usage: format <root path>" << std::endl;
        return 1;
    }

    FileSystem fs(argv[1]);

    if(!fs.format()) {
        std::cout << "ERROR: Unable to format filesystem" << std::endl;
        return 1;
    }

    return 0;
}
