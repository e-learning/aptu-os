#include <iostream>
#include <string>

#include "allocator.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

int main() {
    int n = 0;
    cin >> n;
    Allocator allo(n);
    string buf;

    while (cin >> buf) {
        if (buf == "INFO") {
            allo.Info();
            continue;
        }
        if (buf == "MAP") {
            allo.Map();
            continue;
        }
        int bufint = 0;
        cin >> bufint;
        if (buf == "ALLOC") {
            allo.Alloc(bufint);
        }
        else {
            allo.Free(bufint);
        }
    }
    return 0;
}
