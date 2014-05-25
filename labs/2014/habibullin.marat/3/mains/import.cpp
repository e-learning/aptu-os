#include <iostream>

using std::cout;
using std::endl;

#include "../fsmanager.h"

int main(int argc, char** argv) {
    if(argc != 4) {
        cout << "Usage: import root host_file fs_file" << endl;
        return 0;
    }
    string root(argv[1]);
    string host_file(argv[2]);
    string fs_file(argv[3]);
    FSManager fsm(root, false);
    if(!fsm.Import(root, host_file, fs_file)) {
        return 1;
    }
    fsm.Close(root);
    return 0;
}
