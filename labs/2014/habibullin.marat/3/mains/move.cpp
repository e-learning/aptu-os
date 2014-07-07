#include <iostream>

using std::cout;
using std::endl;

#include "../fsmanager.h"

int main(int argc, char** argv) {
    if(argc != 4) {
        cout << "Usage: move root src dst" << endl;
        return 0;
    }
    string root(argv[1]);
    string src(argv[2]);
    string dst(argv[3]);
    FSManager fsm(root, false);
    if(!fsm.Move(root, src, dst)) {
        return 1;
    }
    fsm.Close(root);
    return 0;
}
