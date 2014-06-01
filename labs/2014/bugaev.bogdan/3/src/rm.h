#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "header.h"
#include "utils.h"
#include "page.h"
#include "file.h"


void removeDataBlock(Config const &config, int number)
{
    if (number <= 0) {
        return;
    }
    Page page;
    page.load(config, number);
    int tmp = getNextDataBlockNumber(page);
    page.unload();
    removeDataBlock(config, tmp);
    pushFreePageNumber(config, number);
}


void removeFile(Config const &config, int number, bool first = true)
{
    if (number <= 0) {
        return;
    }
    Page page;
    page.load(config, number);
    int n1 = getFirstFileNumber(page);
    int n2 = getNextFileNumber(page);
    int n3 = getFirstDataBlockNumber(page);
    page.unload();
    removeFile(config, n1, false);
    if (!first)
        removeFile(config, n2, false);
    removeDataBlock(config, n3);
    pushFreePageNumber(config, number);
}


int rm_main(int argc, char **argv)
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
        int parent = getHeaderSize(config);
        for (size_t i = 0; i + 1 < path.list().size(); ++i) {
            parent = findFileNumber(config, parent, path.list()[i]);
            if (parent == -1) {
                std::cerr << "Path doesn't exist" << std::endl;
                return EXIT_FAILURE;
            }
        }

        Page page;
        page.load(config, parent);

        if (!isDir(page)) {
            std::cerr << "Invalid path" << std::endl;
            return EXIT_FAILURE;
        }

        int fileNumber = findFileNumber(config, parent, path.list().back());
        if (fileNumber == -1) {
            return 0;
        }

        int fn = getFirstFileNumber(page);

        if (fn == fileNumber) {
            Page page2;
            page2.load(config, fn);
            setFirstFileNumber(page, getNextFileNumber(page2));
            page2.unload();
            removeFile(config, fn);
            return 0;
        }

        page.unload();

        while (true) {
            Page page2;
            page2.load(config, fn);
            int nfn = getNextFileNumber(page2);
            page2.unload();
            if (nfn == fileNumber)
                break;
            fn = nfn;
        }
        
        {
            Page page;
            page.load(config, fn);
            Page page2;
            page2.load(config, fileNumber);
            setNextFileNumber(page, getNextFileNumber(page2));
            page2.unload();
            page.unload();
        }

        removeFile(config, fileNumber);

    } catch (Exception const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
