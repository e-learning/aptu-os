#include <iostream>
#include "fs.h"

int main(const int argc, const char *argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cout << "usage: init <root>" << std::endl;
        } else
        {
            file_system fs(argv[1]);
            fs.init();
        }
        return 0;
    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}