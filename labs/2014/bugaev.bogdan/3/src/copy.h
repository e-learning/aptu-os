#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "header.h"
#include "utils.h"
#include "page.h"
#include "file.h"


#include "rm.h"


int copyDataBlock(Config const &config, int number)
{
    if (number <= 0) {
        return number;
    }
    Page page;
    page.load(config, number);
    int tmp = getNextDataBlockNumber(page);
    page.unload();
    int n = copyDataBlock(config, tmp);

    if (n == -2) {
        removeDataBlock(config, n);
        return -2;
    }

    int result = popFreePageNumber(config);
    if (result == -1) {
        removeDataBlock(config, n);
        return -2;
    }

    page.load(config, number);
    Page page2;
    page2.load(config, result);
    PageReader reader(page);
    PageWriter writer(page2);

    while (reader.canRead<char>()) {
        writer.write(reader.read<char>());
    }

    PageWriter(page2).write(n);

    page2.unload();
    page.unload();

    return result;
}


int copyFile(Config const &config, int number, bool first = true)
{
    if (number <= 0) {
        return number;
    }
    Page page;
    page.load(config, number);
    int n1 = getFirstFileNumber(page);
    int n2 = getNextFileNumber(page);
    int n3 = getFirstDataBlockNumber(page);
    page.unload();

    n1 = copyFile(config, n1, false);
    n2 = (!first) ? copyFile(config, n2, false) : -1;
    n3 = copyDataBlock(config, n3);

    if (n1 == -2 || n2 == -2 || n3 == -2) {
        removeFile(config, n1, false);
        removeFile(config, n2, false);
        removeDataBlock(config, n3);
        return -2;
    }

    int result = popFreePageNumber(config);
    if (result == -1) {
        removeFile(config, n1, false);
        removeFile(config, n2, false);
        removeDataBlock(config, n3);
        return -2;
    }

    page.load(config, number);
    Page page2;
    page2.load(config, result);

    PageReader reader(page);
    PageWriter writer(page2);
    while (reader.canRead<char>()) {
        writer.write(reader.read<char>());
    }

    setFirstFileNumber(page2, n1);
    setNextFileNumber(page2, n2);
    setFirstDataBlockNumber(page2, n3);

    page2.unload();
    page.unload();

    return result;
}


int copy_main(int argc, char **argv)
{
    Config config;

    try {
        config = init(argc, argv);
    } catch (ConfigException const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    if (argc != 4) {
        std::cerr << "Illegal arguments" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        Path path(argv[2]);
        int parent = getHeaderSize(config);
        for (size_t i = 0; i < path.list().size(); ++i) {
            parent = findFileNumber(config, parent, path.list()[i]);
            if (parent == -1) {
                std::cerr << "Path doesn't exist" << std::endl;
                return EXIT_FAILURE;
            }
        }

        int firstPage = copyFile(config, parent);
        if (firstPage == -2) {
            std::cerr << "No free space" << std::endl;
            return EXIT_FAILURE;
        }

        {
            Path path(argv[3]);
            int currentFile = getHeaderSize(config);
            for (size_t i = 0; i + 1 < path.list().size(); ++i) {
                currentFile = findFileNumber(config, currentFile, path.list()[i]);
                if (currentFile == -1) {
                    std::cerr << "Path doesn't exist" << std::endl;
                    return EXIT_FAILURE;
                }
            }

            Page page;
            page.load(config, currentFile);

            if (!isDir(page)) {
                std::cerr << "Invalid path" << std::endl;
                return EXIT_FAILURE;
            }

            page.unload();

            if (findFileNumber(config, currentFile, path.list().back()) != -1) {
                std::cerr << "File already exists" << std::endl;
                return EXIT_FAILURE;
            }

            page.load(config, currentFile);
            Page page2;
            page2.load(config, firstPage);

            PageWriter writer(page2);
            writer.step<int>(4).write(time(0));
            writer.write<int>(path.list().back().size()).writeString(path.list().back());

            PageWriter(page2).step<int>().write(getFirstFileNumber(page));
            PageWriter(page).write(firstPage);
            page.unload();
        }

    } catch (Exception const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
