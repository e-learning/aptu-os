#include <iostream>
#include <unistd.h>
#include "filesystem.h"

int main (const int argc, const char *argv[]) {
    try {
        if (argc < 2) {
            std::cout << "Usage: init root" << std::endl;
        } else {
            FileSystem fs(argv[1]);
            ConfigInfo config = fs.readConfig();
            for (int i = 0; i < config.numberOfBlocks; ++i) {
                const string blockName = fs.getBlockById(i);
                ofstream(blockName, ios::trunc).close();
                if (truncate(blockName.c_str(), config.blockSize)) {
                    throw "Cannot create blocks";
                }
            }
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
