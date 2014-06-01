#include <iostream>
#include <cstdlib>

#include "config.h"
#include "header.h"
#include "utils.h"
#include "page.h"
#include "file.h"


int main(int argc, char **argv)
{
    Config config;

    try {
        config = init(argc, argv);
    } catch (ConfigException const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    if (argc != 3) {
        std::cerr << "Illegal arguments" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        Path path(argv[2]);
        int currentDir = getHeaderSize(config);
        size_t i = 0;

        for (; i < path.list().size(); ++i) {
            int file = findFileNumber(config, currentDir, path.list()[i]);
            if (file == -1)
                break;
            currentDir = file;
        }

        while (true) {
            if (i == path.list().size())
                return 0;

            Page page;
            page.load(config, currentDir);
            if (!isDir(page)) {
                std::cerr << "Invalid path" << std::endl;
                return EXIT_FAILURE;
            }

            int newDir = popFreePageNumber(config);
            if (newDir == -1) {
                std::cerr << "No free space" << std::endl;
                return EXIT_FAILURE;
            }

            createDir(config, newDir, path.list()[i]);
            Page page2;
            page2.load(config, newDir);
            PageWriter(page2).step<int>().write(getFirstFileNumber(page));
            PageWriter(page).write(newDir);
            page2.unload();

            page.unload();
            currentDir = newDir;
            ++i;
        }
    } catch (Exception const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
