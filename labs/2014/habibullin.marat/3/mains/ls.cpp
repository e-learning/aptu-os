#include <iostream>

using std::cout;
using std::endl;

#include "../fsmanager.h"

int main(int argc, char** argv) {
    if(argc != 3) {
        cout << "Usage: ls root path" << endl;
        return 0;
    }
    string root(argv[1]);
    string path(argv[2]);
    FSManager fsm(root, false);
    if(!fsm.Ls(root, path)) {
        return 1;
    }
    fsm.Close(root);
    return 0;
}
