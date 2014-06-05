#include <iostream>
#include "filesystem.h"

int main (const int argc, const char *argv[]) {
    try {
        if (argc < 2) {
            std::cout << "Usage: format root" << std::endl;
            return 0;
        } else {
            FileSystem(argv[1]).format();
        }
        return 0;
    } catch (const char *message) {
        std::cerr << message << std::endl;
        return 1;
    } catch (const string msgstr) {
        std::cerr << msgstr << std::endl;
        return 1;
    }
}
