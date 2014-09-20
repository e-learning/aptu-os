#include "HasArgsCmd.h"
#include "../utils/StringUtils.h"

void HasArgsCmd::parseArgs(vector<string> &args, string command) {
    utils::split(utils::trim(command), ' ', args);
}

void HasArgsCmd::exec() {
    vector<string> args;
    parseArgs(args, text);
    exec(args);
}
