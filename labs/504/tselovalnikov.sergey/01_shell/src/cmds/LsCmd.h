#pragma once

#include <vector>
#include "Cmd.h"
#include "HasArgsCmd.h"

class LsCmd : public HasArgsCmd {
public:
    LsCmd(string text) : HasArgsCmd(text) {
    }

    virtual void exec(vector<string> &args);
};