#include "Cmd.h"
#include "HasArgsCmd.h"

class KillCmd : public HasArgsCmd {
public:
    KillCmd(string text) : HasArgsCmd(text) {
    }

    virtual void exec(vector<string> &args);
};