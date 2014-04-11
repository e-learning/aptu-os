#include "Const.h"
#include "command.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        print_error(ERROR_FEW_ARGS);
        return ERROR_FEW_ARGS;
    }
    std::string root = argv[1];
   
    size_t result = format(root);
    if (result == ERROR_READ_CONFIG) {
        std::cout << "Can't read config..." << std::endl;
        return ERROR_READ_CONFIG;
    }
    return SUCCESS;
}
