#pragma once

#include "Cmd.h"

class ExitCmd : public Cmd {
public:
    ExitCmd(string text) : Cmd(text) {
    }

    virtual string exec();
};