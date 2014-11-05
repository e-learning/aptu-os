#include <iostream>
#include "fs.h"

int main(const int argc, const char *argv[])
{
    try
    {
        if (argc < 3)
        {
            std::cout << "usage: rm <root> <file>" << std::endl;
            return 0;
        } else
        {
            file_system(argv[1]).rm(argv[2]);
        }
        return 0;
    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}