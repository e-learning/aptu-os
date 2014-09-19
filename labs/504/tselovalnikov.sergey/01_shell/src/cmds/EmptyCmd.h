#include "Cmd.h"

class EmptyCmd : public Cmd {
public:
    EmptyCmd() : Cmd("") {
    }

    virtual string exec();
};