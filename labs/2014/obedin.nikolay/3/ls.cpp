#include <iostream>
using namespace std;
#include "filesystem.hpp"

int main(int argc, const char *argv[]) {
    argc--;
    argv++;
    if (argc < 2) {
        cerr << "Usage: ls <path> <fs_path>" << endl;
        return 1;
    }

    try {
        filesystem fs(argv[0], true);
        cout << fs.cmd_ls(argv[1]);
    } catch (filesystem::error e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
