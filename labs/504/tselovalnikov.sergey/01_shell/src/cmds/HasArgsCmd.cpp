#include "HasArgsCmd.h"
#include "../utils/StringUtils.h"

void HasArgsCmd::parseArgs(vector<string> &args, string command) {
    utils::split(utils::trim(command), ' ', args);
}

string HasArgsCmd::exec() {
    vector<string> args;
    parseArgs(args, text);
    return exec(args);
}
