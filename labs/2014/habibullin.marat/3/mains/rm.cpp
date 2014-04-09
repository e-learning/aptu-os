#include <iostream>

using std::cout;
using std::endl;

#include "../fsmanager.h"

int main(int argc, char** argv) {
    if(argc != 3) {
        cout << "Usage: rm root file" << endl;
        return 0;
    }
    string root(argv[1]);
    string file(argv[2]);
    FSManager fsm(root, false);
    if(!fsm.Rm(root, file)) {
        return 1;
    }
    fsm.Close(root);
    return 0;
}
