#include <iostream>
#include "file_system.h"

using namespace std;

int main(const int argc, const char *argv[]) try {
    if (argc < 4) {
        std::cout << "Usage: move root source destination" << std::endl;
    } else {
        file_system fs = file_system(argv[1]);

        file_descriptor source_d = fs.find_descriptor(argv[2], false);
        file_descriptor destination_d = fs.find_descriptor(argv[3], true, source_d.directory);

        fs.move(source_d, destination_d);
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
} catch (const string msg) {
    std::cerr << msg << std::endl;
    return 1;
}
