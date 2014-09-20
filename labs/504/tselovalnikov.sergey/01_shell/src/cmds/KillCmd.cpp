#include "KillCmd.h"
#include <vector>
#include <iostream>
#include <csignal>
#include <cstdlib>


void KillCmd::exec(vector<string> &args) {
    if(args.size() <= 1) {
        cout << "Please provide PID as first parameter to kill" << endl << flush;
        return;
    }
    string pid = args[1];
    kill(atoi(pid.c_str()), SIGTERM);
}
