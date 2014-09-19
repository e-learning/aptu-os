#include "Cmd.h"
#include "HasArgsCmd.h"

class KillCmd : public HasArgsCmd {
public:
    KillCmd(string text) : HasArgsCmd(text) {
    }

    virtual string exec(vector<string> &args);
};