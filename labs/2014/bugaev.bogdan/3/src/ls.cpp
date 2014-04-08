#include <iostream>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "header.h"
#include "utils.h"
#include "page.h"
#include "file.h"


void printFileInfo(Page &page)
{
    std::cout << getFileName(page) << '\t';
    std::cout << getFileSize(page) << '\t';
    time_t date = getFileDate(page);
    std::cout << asctime(localtime(&date));
}


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
        int currentFile = getHeaderSize(config);
        for (size_t i = 0; i < path.list().size(); ++i) {
            currentFile = findFileNumber(config, currentFile, path.list()[i]);
            if (currentFile == -1) {
                std::cerr << "File doesn't exist" << std::endl;
                return EXIT_FAILURE;
            }
        }

        Page page;
        page.load(config, currentFile);
        printFileInfo(page);

        if (!isDir(page)) {
            return 0;
        }

        std::cout << getFileName(page) << ":" << std::endl;

        currentFile = getFirstFileNumber(page);
        while (currentFile != -1) {
            page.load(config, currentFile);
            printFileInfo(page);
            currentFile = getNextFileNumber(page);
        }

        page.unload();
    } catch (Exception const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
