#pragma once

#include <vector>
#include "Cmd.h"

class HasArgsCmd : public Cmd {
public:
    HasArgsCmd(string text) : Cmd(text) {
    }

    string exec();

    virtual string exec(vector<string> &args) = 0;
protected:
    void parseArgs(vector<string> &args, string command);
};
