#include "../main/filesystem.h"
/**
 * create blocks for FS depends on config file
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "wrong arg number! Usage: init <path>" << std::endl;
        return 1;
    }

    File_system fs(argv[1]);
    if(fs.init() == 0) {
        std::cout << "failed on init" << std::endl;
        return 1;
    }

    return 0;
}

