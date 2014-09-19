#pragma once

#include "Cmd.h"
#include "HasArgsCmd.h"

class ExecCmd : public HasArgsCmd {
public:
    ExecCmd(string text) : HasArgsCmd(text) {
    }

private:

    virtual string exec(vector<string> &args);

    void transformArgs(vector<string> &args, vector<char *> &argv);

};