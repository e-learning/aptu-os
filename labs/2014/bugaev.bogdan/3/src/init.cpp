#include <iostream>
#include <fstream>
#include <cstdlib>

#include "config.h"
#include "utils.h"


int main(int argc, char **argv)
{
    Config config;

    try {
        config = init(argc, argv);
    } catch (ConfigException const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < config.pageCount; ++i) {
        std::ofstream fout(config.root.concat(getString(i)).c_str());
        if (!fout) {
            std::cerr << "Error" << std::endl;
            return EXIT_FAILURE;
        }
        for (int j = 0; j < config.pageSize; ++j) {
            fout.put(0);
        }
    }

    return 0;
}
