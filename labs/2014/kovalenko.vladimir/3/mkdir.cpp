#include <iostream>
#include "filesystem.h"

using namespace std;

int main (const int argc, const char *argv[]) {
    try {
        if (argc < 3) {
            std::cout << "Usage: mkdir root path" << std::endl;
        } else {
            FileSystem(argv[1]).findDir(argv[2], true);
        }
        return 0;
    } catch (const char *message) {
        std::cerr << message << std::endl;
        return 1;
    } catch (const string msg) {
        std::cerr << msg << std::endl;
        return 1;
    }
}
