#include <iostream>

using std::cout;
using std::endl;

#include "../fsmanager.h"

int main(int argc, char** argv) {
    if(argc != 4) {
        cout << "Usage: export root fs_file host_file" << endl;
        return 0;
    }
    string root(argv[1]);
    string fs_file(argv[2]);
    string host_file(argv[3]);
    FSManager fsm(root, false);
    if(!fsm.Export(root, fs_file, host_file)) {
        return 1;
    }
    fsm.Close(root);
    return 0;
}

