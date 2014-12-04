#include <iostream>
#include <exception>
#include "file_system.h"

int main(const int argc, const char *argv[])
{

    try
    {
        if (argc < 2)
        {
            std::cout << "Usage: format <root>" << std::endl;
            return 0;
        } else
        {
            file_system(argv[1]).format();
        }
        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

}