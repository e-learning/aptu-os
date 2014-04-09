#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

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

    if (argc != 4) {
        std::cerr << "Illegal arguments" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        Path path(argv[2]);
        int currentFile = getHeaderSize(config);
        for (size_t i = 0; i < path.list().size(); ++i) {
            currentFile = findFileNumber(config, currentFile, path.list()[i]);
            if (currentFile == -1) {
                std::cerr << "Path doesn't exist" << std::endl;
                return EXIT_FAILURE;
            }
        }

        std::ofstream dst(argv[3], std::ofstream::binary);
        if (!dst) {
            std::cerr << "Couldn't open output file" << std::endl;
            return EXIT_FAILURE;
        }

        Page page;
        page.load(config, currentFile);
        if (isDir(page)) {
            std::cerr << "Invalid path" << std::endl;
            return EXIT_FAILURE;
        }

        PageReader reader(page);
        int fileSize = reader.step<int>(3).read<int>();
        reader.step<time_t>();
        int nameSize = reader.read<int>();
        reader.step<char>(nameSize);

        while (reader.canRead<char>() && fileSize) {
            dst.put(reader.read<char>());
            --fileSize;
        }

        currentFile = PageReader(page).step<int>(2).read<int>();
        page.unload();

        while (currentFile != -1) {
            Page page;
            page.load(config, currentFile);
            PageReader reader(page);
            reader.step<int>();
            while (fileSize && reader.canRead<char>()) {
                dst.put(reader.read<char>());
                --fileSize;
            }
            currentFile = PageReader(page).read<int>();
        }

    } catch (Exception const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
