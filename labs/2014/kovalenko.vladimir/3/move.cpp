#include <iostream>
#include "filesystem.h"

using namespace std;

int main (const int argc, const char *argv[]) {
    try {
        if (argc < 4) {
            std::cout << "Usage: move root source destination" << std::endl;
        } else {
            FileSystem fs = FileSystem(argv[1]);
            FileEntry sourceEntry = fs.findEntry(argv[2], false);
            FileEntry destEntry = fs.findEntry(argv[3], true, sourceEntry.dir);
            fs.move(sourceEntry, destEntry);
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
