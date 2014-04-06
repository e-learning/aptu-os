#include <iostream>
#include "fs.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 4) {
        std::cout << "Usage: move root source destination" << std::endl;
    } else {
        FS fs = FS(argv[1]);

        FileDescriptor source_d = fs.find_descriptor(argv[2], false);
        FileDescriptor destination_d = fs.find_descriptor(argv[3], true, source_d.directory);

        if (source_d.directory) {
            if (destination_d.directory) {
                if (source_d.first_child != -1) {
                    // move all files from source to destination
                    FileDescriptor c = fs.read_descriptor(source_d.first_child);
                    while (c.next_file != -1) {
                        fs.remove_descriptor(c.id);
                        fs.insert_child(destination_d, c);
                        c = fs.read_descriptor(c.next_file);
                    }
                    fs.remove_descriptor(c.id);
                    fs.insert_child(destination_d, c);
                }
                fs.remove_descriptor(source_d.id);
            } else {    // cannot move directory to a file
                throw (string(argv[3]) + " is a file").c_str();
            }
        } else {        // source is a file
            if (destination_d.directory) {
                fs.remove_descriptor(source_d.id);
                fs.insert_child(destination_d, source_d);
            } else {    // link destination data to source
                fs.clear_descriptor(destination_d);
                destination_d.number_of_blocks = source_d.number_of_blocks;
                destination_d.first_block = source_d.first_block;
                destination_d.time = time(0);
                fs.remove_descriptor(source_d.id);
                fs.write_descriptor(destination_d);
            }
        }
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
