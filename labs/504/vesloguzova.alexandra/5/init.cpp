#include <iostream>
#include <exception>
#include "file_system.h"

int main(const int argc, const char *argv[])
{

    try
    {
        if (argc < 2)
        {
            std::cout << "Usage: init <root>" << std::endl;
        } else
        {
            file_system fs(argv[1]);
            fs.init();

        }
        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}