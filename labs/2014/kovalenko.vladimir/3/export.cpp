#include <iostream>
#include "filesystem.h"

using namespace std;

int main (const int argc, const char *argv[]) {
    try {
        if (argc < 4) {
            std::cout << "Usage: export root path_in_filesystem host_path" << std::endl;
        } else {
            FileSystem fileSystem = FileSystem(argv[1]);
            FileEntry fileEntry = fileSystem.findEntry(argv[2], false);

            ofstream outputFile(argv[3], ios::binary | ios::trunc);
            if (!outputFile) {
                throw "Cannot open file";
            }

            fileSystem.readData(fileEntry,outputFile);
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
