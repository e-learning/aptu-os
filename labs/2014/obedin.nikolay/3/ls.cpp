#include <iostream>
using namespace std;
#include "filesystem.hpp"
#include <iomanip>

int main(int argc, const char *argv[]) {
    argc--;
    argv++;
    if (argc < 2) {
        cerr << "Usage: ls <root> <fs_path>" << endl;
        return 1;
    }

    try {
        filesystem fs(argv[0], true);
        string result = fs.cmd_ls(argv[1]);
        cout << "T | " << setw(11) << "Filename"
             << " |   Size | Timestamp" << endl;
        cout << setfill('-') << setw(44) << "-" << endl;
        cout << result;
    } catch (filesystem::error e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
