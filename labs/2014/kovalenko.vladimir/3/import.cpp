#include <iostream>
#include "filesystem.h"
#include <string>

using std::string;
using std::ifstream;

int main (const int argc, const char *argv[]) {
    try {
        if (argc < 4) {
            std::cout << "Usage: import root host_file fs_file" << std::endl;
            return 0;
        } else {
            FileSystem fs(argv[1]);

            fs.checkInit();

            ifstream source(argv[2], ios::binary);
            if (!source) {
                throw "Cannot open file to import";
            }

            FileEntry entry = fs.findEntry(argv[3]);
            if (entry.dir) {
                throw string("'") + entry.name + "' is a directory";
            }

            fs.writeData(entry,source);
        }
        return 0;
    } catch (const char * msg) {
        std::cerr << msg << std::endl;
        return 1;
    } catch (const string msg) {
        std::cerr << msg << std::endl;
    }
}
