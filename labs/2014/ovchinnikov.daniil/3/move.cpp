#include <iostream>
#include "fs.h"

using namespace std;

int main (const int argc, const char *argv[]) try {
    if (argc < 4) {
        std::cout << "Usage: import root host_file fs_file" << std::endl;
        return 0;
    } else {
        FS fs = FS(argv[1]);

        FileDescriptor source_d = fs.find_descriptor(argv[2], false);
        FileDescriptor desctination_d = fs.find_descriptor(argv[3], true, source_d.directory); //create file

        if (source_d.directory) {
            if (desctination_d.directory) {
                if (source_d.first_child != -1) {
                    FileDescriptor c = fs.read_descriptor(source_d.first_child);
                    while (c.next_file != -1) {
                        fs.remove_descriptor(c);
                        fs.insert_child(desctination_d, c);
                        c = fs.read_descriptor(c.next_file);
                    }
                    fs.remove_descriptor(c);
                    fs.insert_child(desctination_d, c);
                }
                fs.remove_descriptor(source_d);
            } else {
                throw (string(argv[3]) + " is a file").c_str();
            }
        } else {        // source is a file
            if (desctination_d.directory) {
                fs.remove_descriptor(source_d);
                fs.insert_child(desctination_d, source_d);
            } else {    // link destination data to source
                fs.clear_descriptor(desctination_d);
                desctination_d.first_block = source_d.first_block;
                desctination_d.time = time(0);
                fs.remove_descriptor(source_d);
                fs.write_descriptor(desctination_d);
            }
        }
    }
    return 0;
} catch (const char * msg) {
    std::cerr << msg << std::endl;
    return 1;
}
