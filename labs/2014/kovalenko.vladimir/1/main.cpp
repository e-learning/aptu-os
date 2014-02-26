#include <iostream>
#include <sstream>
#include "Executor.h"

Command parse(std::string const &str)
{
    Command result;
    std::istringstream stream(str);
    stream >> result.cmd;
    std::string arg;
    while (stream >> arg) {
        result.args.push_back(arg);
    }
    if (result.args.empty()) result.hasNoArguments = true;
    else result.hasNoArguments = false;
    return result;
}

int main()
{
	Executor * ex = new Executor();
    while (true) {
        std::cout << "poorShell>> ";
        std::string str;
        std::getline(std::cin, str);
        if (!ex->execute(parse(str)))
            break;
    }
    return 0;
}

