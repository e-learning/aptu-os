#include "command.h"
#include <cstdlib>
#include <sstream>
#include <string>

bool Parser::parse_alloc(vecStr const & vCmd)
{
    if ((vCmd.size() == 2) && (vCmd[0] == "ALLOC")) {
        return true;
    }
    return false;
}

bool Parser::parse_free(vecStr const & vCmd)
{
    if ((vCmd.size() == 2) && (vCmd[0] == "FREE")) {
        return true;
    }
    return false;
}

bool Parser::parse_info(vecStr const & vCmd)
{
    if ((vCmd.size() == 1) && (vCmd[0] == "INFO")) {
        return true;
    }
    return false;
}

bool Parser::parse_map(vecStr const & vCmd)
{
    if ((vCmd.size() == 1) && (vCmd[0] == "MAP")) {
        return true;
    }
    return false;
}

Command* Parser::parse (vecStr const & vCmd)
{
    if (parse_alloc(vCmd)) {
        return new CommandAlloc(atoi(vCmd[1].c_str()));
    }
    if (parse_free(vCmd)) {
        return new CommandFree(atoi(vCmd[1].c_str()));
    }
    if (parse_info(vCmd)) {
        return new CommandInfo();
    }
    if (parse_map(vCmd)) {
        return new CommandMap();
    }
    return new Command();
}
