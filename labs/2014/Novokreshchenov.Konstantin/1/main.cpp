#include "Parser.h"

int main()
{
    std::vector<CommandParser*> parsers;
    create_parsers (parsers);

    while (true) {
        print_prompt ();

        std::string input;
        std::vector<std::string> command;
        std::getline (std::cin, input);
        parse_input (input, command);

        if (command.empty()) {
            continue;
        }

        int res;
        for (size_t i = 0; i < parsers.size(); ++i)
        {
            res = parsers[i]->parse(command);
            if (res) {
                break;
            }
        }
        if (res == EXIT) {
            break;
        }
        if (res == ERROR) {
            std::cout << "shell: Error occured" << std::endl;
        }
    }

    delete_parsers (parsers);

    return 0;
}

