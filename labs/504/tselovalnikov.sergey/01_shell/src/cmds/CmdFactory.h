#pragma once

#include "Cmd.h"
#include <string>

class CmdFactory {
public:
    Cmd* getCmd(string line);
};

