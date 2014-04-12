#include "manager.h"
#include "command.h"
#include "utility.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>


int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Few arguments! Use: allocator <blocksize>" << std::endl;
        return ERROR;
    }
    size_t memorySize = atoi (argv[1]);
    Parser parser;
    MemoryManager memoryManager (memorySize);

    while (true) {
        std::vector<std::string> vCmd;
        get_command (vCmd);
        Command* cmd = parser.parse(vCmd);
        cmd->accept(memoryManager);
        delete cmd;
    }

    return 0;
}

