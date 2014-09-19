#pragma once

#include "Cmd.h"

class PsCmd : public Cmd {
public:
    PsCmd(string text) : Cmd(text) {
    }

    virtual string exec();
};