#ifndef __FS_H
#define __FS_H

#include <vector>
#include <fstream>
#include "data.h"

using std::string;
using std::ifstream;

class FS {

public:

    FS(const char *root) : root(root), config(Config()) {
        ifstream config_file((string(root) + "/config").c_str());
        if (!config_file.good()) {
            throw "Cannot open config file";
        }
        config_file  >> config.block_size >> config.block_number;
        if (!config_file.good()
                || config.block_size > 50000000 || config.block_size < 1024
                || config.block_number > 50000 || config.block_number < 1) {
            throw "Config file is bad";
        }
    }
    ~FS() {}

    int init();
    int format();
    int import(const char *filename, const char *destination);

private:
    const char * root;
    Config config;

    std::vector<bool> read_block_map();
    void writeBlockMap(const std::vector<bool> &);

    const char *get_block_f(const int id) const {
        return (string(root) +"/"+ std::to_string(id)).c_str();
    }
};


#endif // __FS_H
