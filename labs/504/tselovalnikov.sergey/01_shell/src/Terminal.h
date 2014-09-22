#include <string>
#include "cmds/CmdFactory.h"

using namespace std;

class Terminal {
public:
    Terminal();

    ~Terminal();

    void run();

protected:
    string read();

    void eval(string line);

private:
    CmdFactory *cmdFactory;

    bool hasPipe(string line);

    void execWithPipe(string line);

    void exec(string line);

    bool hasRedirectOut(string line);

    string setupRedirect(string line);
};