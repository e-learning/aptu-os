#include "KillCmd.h"
#include <vector>
#include <iostream>
#include <csignal>
#include <cstdlib>


string KillCmd::exec(vector<string> &args) {
    if(args.size() <= 1) {
        cout << "Please provide PID as first parameter to kill" << endl;
        return "";
    }
    string pid = args[1];
    kill(atoi(pid.c_str()), SIGTERM);
    return "SIGTERM sended";
}
