#include "CmdFactory.h"
#include "PwdCmd.h"
#include "ExecCmd.h"
#include "ExitCmd.h"
#include "LsCmd.h"
#include "../utils/StringUtils.h"
#include "KillCmd.h"
#include "PsCmd.h"
#include "EmptyCmd.h"

Cmd* CmdFactory::getCmd(string line) {
    vector<string> args;
    utils::split(utils::trim(line), ' ', args);
    string cmdName = args[0];
    if (cmdName == "pwd") {
        return new PwdCmd(line);
    }
    if (cmdName == "exit") {
        return new ExitCmd(line);
    }
    if (cmdName == "ls") {
        return new LsCmd(line);
    }
    if (cmdName == "kill") {
        return new KillCmd(line);
    }
    if (cmdName == "ps") {
        return new PsCmd(line);
    }
    return new ExecCmd(line);
}