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
    std::string strMemorySize = "";
    std::getline(std::cin, strMemorySize);
    size_t memorySize = atoi(strMemorySize.c_str());
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


