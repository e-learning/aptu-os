#include <iostream>
#include <cstdlib>

#include "config.h"
#include "utils.h"
#include "file.h"
#include "page.h"


int main(int argc, char **argv)
{
    Config config;

    try {
        config = init(argc, argv);
    } catch (ConfigException const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    int tmp = sizeof(int) * (2 + config.pageCount);
    int headerSize = tmp / config.pageSize + (tmp % config.pageSize != 0);
    if (headerSize + 1 > config.pageCount) {
        std::cerr << "Error" << std::endl;
        return EXIT_FAILURE;
    }

    Page page;
    try {
        int currentPage = 0;
        int i = headerSize + 1;
        while (i < config.pageCount) {
            Page page;
            page.load(config, currentPage);
            PageWriter writer(page);

            if (currentPage == 0) {
                writer.write(headerSize);
                writer.write(config.pageCount - headerSize - 1);
            }
            while (i < config.pageCount && writer.canWrite<int>()) {
                writer.write(i++);
            }
            
            ++currentPage;
        }

        createDir(config, currentPage, "/");
    } catch (PageException const &e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
