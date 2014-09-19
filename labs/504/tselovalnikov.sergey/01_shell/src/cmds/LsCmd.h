#pragma once

#include <vector>
#include "Cmd.h"
#include "HasArgsCmd.h"

class LsCmd : public HasArgsCmd {
public:
    LsCmd(string text) : HasArgsCmd(text) {
    }

    virtual string exec(vector<string> &args);
};