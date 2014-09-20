#pragma once

#include "Cmd.h"

class PwdCmd : public Cmd {
public:
    PwdCmd(string text) : Cmd(text) {
    }

    virtual void exec();
};