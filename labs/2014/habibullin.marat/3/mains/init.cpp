#include <iostream>

using std::cout;
using std::endl;

#include "../fsmanager.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "Usage: init root" << endl;
        return 0;
    }
    string root(argv[1]);
    FSManager fsm(root, true);
    fsm.Init(root);
    fsm.Close(root);
    return 0;
}
