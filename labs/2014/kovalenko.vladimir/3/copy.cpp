#include <iostream>
#include "filesystem.h"

using namespace std;

int main (const int argc, const char *argv[]){
    try {
        if (argc < 4) {
            std::cout << "Usage: copy root source destination" << std::endl;
        } else {
            FileSystem fs = FileSystem(argv[1]);

            FileEntry source_d = fs.findEntry(argv[2], false);
            FileEntry destination_d = fs.findEntry(argv[3], true, source_d.dir);

            fs.copy(source_d, destination_d);
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
