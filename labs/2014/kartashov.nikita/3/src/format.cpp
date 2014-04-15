#include <iostream>
#include "utils/file_system.h"

int main (const int argc, const char *argv[]) try {
    if (argc < 2) {
        std::cout << "Usage: format root" << std::endl;
        return 0;
    } else {
        file_system(argv[1]).format();
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
} catch (const string msg) {
    std::cerr << msg << std::endl;
    return 1;
}
