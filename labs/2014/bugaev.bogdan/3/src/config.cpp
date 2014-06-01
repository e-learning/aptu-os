#include "config.h"
#include "defs.h"
#include <fstream>


Config const init(int argc, char **argv)
{
    if (argc < 2) {
        throw ConfigException("Illegal arguments");
    }

    Config config;
    config.root.init(argv[1]);

    std::ifstream fin(config.root.concat("config").c_str());
    if (!fin) {
        throw ConfigException("Couldn't open config");
    }

    fin >> config.pageSize >> config.pageCount;

    if (config.pageSize < MIN_PAGE_SIZE || config.pageSize > MAX_PAGE_SIZE
            || config.pageSize * config.pageCount > MAX_DISK_SIZE) {
        throw ConfigException("Illegal config");
    }

    return config;
}
