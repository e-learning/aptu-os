#include <iostream>
#include "filesystem.h"

using namespace std;

int main (const int argc, const char *argv[]) {
    try {
        if (argc < 3) {
            std::cout << "Usage: rm root path" << std::endl;
            return 0;
        } else {
            FileSystem fs = FileSystem(argv[1]);
            FileEntry entry = fs.findEntry(argv[2], false);
            fs.rm(entry);
        }
        return 0;
    } catch (const char * msg) {
        std::cerr << msg << std::endl;
        return 1;
    } catch (const string msg) {
        std::cerr << msg << std::endl;
        return 1;
    }
}

