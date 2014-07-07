#include <iostream>

using std::cout;
using std::endl;

#include "../fsmanager.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "Usage: format root" << endl;
        return 0;
    }
    string root(argv[1]);
//    string root("../SFS/tst");
    FSManager fsm(root, false);
    if(!fsm.Format(root)) {
        return 1;
    }
    fsm.Close(root);
    return 0;
}
