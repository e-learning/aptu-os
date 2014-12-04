#include <iostream>
#include <exception>
#include "file_system.h"

int main(const int argc, const char *argv[])
{

    try
    {
        if (argc < 4)
        {
            std::cout << "Usage: import <root> <host_file> <system_file>" << std::endl;
            return 0;
        } else
        {
            file_system(argv[1]).import(argv[2], argv[3]);
        }
        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
