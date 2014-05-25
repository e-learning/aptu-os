#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "header.h"
#include "utils.h"
#include "page.h"
#include "file.h"
#include "defs.h"


int main(int argc, char **argv)
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
        Path path(argv[3]);
        int currentFile = getHeaderSize(config);
        for (size_t i = 0; i + 1 < path.list().size(); ++i) {
            currentFile = findFileNumber(config, currentFile, path.list()[i]);
            if (currentFile == -1) {
                std::cerr << "Path doesn't exist" << std::endl;
                return EXIT_FAILURE;
            }
        }

        std::ifstream src(argv[2], std::ifstream::binary);
        if (!src) {
            std::cerr << "File doesn't exist" << std::endl;
            return EXIT_FAILURE;
        }

        {
            src.seekg(0, src.end);
            int len = src.tellg();
            src.seekg(0, src.beg);
            if (len / config.pageSize + 1 > getFreePageCount(config)) {
                std::cerr << "No free space" << std::endl;
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

        int firstPage = popFreePageNumber(config);
        if (firstPage == -1) {
            std::cerr << "No free space" << std::endl;
            return EXIT_FAILURE;
        }

        Page page2;
        page2.load(config, firstPage);
        PageWriter writer(page2);
        writer.write(0).write(-1).write(-1);
        writer.step<int>().write(time(0));
        writer.write<int>(LEN/*path.list().back().size()*/).writeString(path.list().back());
        //writer.step<char>(LEN - (int) path.list().back().size()); // !!!

        PageWriter(page2).step<int>().write(getFirstFileNumber(page));
        PageWriter(page).write(firstPage);
        page.unload();

        int fileSize = 0;
        while (src.good() && writer.canWrite<char>()) {
            char byte;
            src.get(byte);
            writer.write(byte);
            ++fileSize;
        }

        int curPage = firstPage;
        while (src.good()) {
            int newPage = popFreePageNumber(config);
            if (newPage == -1) {
                std::cerr << "No free space" << std::endl;
                return EXIT_FAILURE;
            }

            if (curPage == firstPage) {
                PageWriter(page2).step<int>(2).write(newPage);
            } else {
                Page page;
                page.load(config, curPage);
                PageWriter(page).write(newPage);
                page.unload();
            }

            Page page;
            page.load(config, newPage);
            PageWriter writer(page);
            writer.write(-1);
            while (src.good() && writer.canWrite<char>()) {
                char byte;
                src.get(byte);
                writer.write(byte);
                ++fileSize;
            }
            page.unload();

            curPage = newPage;
        }

        PageWriter(page2).step<int>(3).write(fileSize - 1);
        page2.unload();
    } catch (Exception const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
