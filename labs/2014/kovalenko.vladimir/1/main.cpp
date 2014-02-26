#include <iostream>
#include <sstream>
#include "Executor.h"





Command parse(std::string const &str)
{
    Command result;
    // result.raw = str;

    std::istringstream stream(str);
    stream >> result.cmd;

    std::string arg;
    while (stream >> arg) {
        result.args.push_back(arg);
    }

    return result;
}

int main()
{
	Executor * ex = new Executor();
    while (true) {
        std::cout << "$ ";

        std::string str;
        std::getline(std::cin, str);

        if (!ex->execute(parse(str)))
            break;
    }

    return 0;
}

