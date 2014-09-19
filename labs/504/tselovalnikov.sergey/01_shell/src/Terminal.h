#pragma once

#include <string>
#include "cmds/CmdFactory.h"

using namespace std;

class Terminal {
public:
    Terminal();

    void run();

protected:
    string read();

    string eval(string line);

    void print(string str);

private:
    CmdFactory *cmdFactory;

    bool hasPipe(string line);

    string execWithPipe(string line);

    string exec(string line);

    bool hasRedirectOut(string line);

    string setupRedirect(string line);
};