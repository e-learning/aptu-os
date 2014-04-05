#include <iostream>
#include "fs.h"

using namespace std;

int main(const int argc, const char* argv[]) {
    if (argc < 4) {
        cout << "Usage: import root host_file fs_file" << endl;
        return 0;
    } else {
        return FS(argv[1]).import(argv[2], argv[3]);
    }
}
