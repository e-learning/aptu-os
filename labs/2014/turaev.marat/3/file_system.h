#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "structs.h"
#include <vector>
#include <fstream>
#include <sstream>

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;

class file_system {
public:
    file_system(const char *root);
    ~file_system();

    void format();
    void rm(const file_descriptor &fd);
    void copy(const file_descriptor &, file_descriptor &);
    void move(const file_descriptor &, file_descriptor &);
    vector<file_descriptor> list(const file_descriptor & directory) const;

    file_descriptor find_descriptor(const char *destination, bool create = true, bool is_directory = false);
    file_descriptor find_directory(const char *destination, bool create = true);

    void read_data(const file_descriptor &, std::ostream &);
    void write_data(file_descriptor &, std::istream &);

    configuration get_config();

    template <typename D = file_descriptor>
    D read_descriptor(int id) const {
        D fd;
        ifstream block(get_block_f(id), ios::binary);
        block.read((char *) &fd, sizeof (D));
        if (block) {
            return fd;
        } else {
            throw "Cannot read descriptor ";
        }
    }

    void write_descriptor(const file_descriptor & d) {
        if (d.id == 0) {
            root_d = d;
        }
        ofstream block(get_block_f(d.id), ios::binary);
        block.write((char *) &d, sizeof (file_descriptor));
        if (!block) {
            throw "Cannot write descriptor";
        }
        block_map[d.id] = true;
    }

    void check_initialized() {
        if (!initialized) {
            throw "Filesystem isn't initialized yet";
        }
    }

    const string get_block_f(const int id) const {
        return string(root) + "/" + std::to_string(id);
    }

private:
    const char *root;
    const configuration config;
    const int BLOCK_DATA_SIZE;
    const bool initialized;
    file_descriptor root_d;
    vector<bool> block_map;

    file_descriptor init_descriptor(const string &name, bool directory = true);
    void clear_descriptor(const file_descriptor &fd);
    void remove_descriptor(const int);

    file_descriptor find_descriptor(file_descriptor & directory, const string & name, bool create = true, bool is_directory = false);
    file_descriptor find_directory(const vector<string> & path, bool create = true);

    void read_block_map();
    void write_block_map();

    size_t find_first_free_block(const int start = 0) const {
        for (size_t i = start + 1; i < block_map.size(); i++) {
            if (!block_map[i]) {
                return i;
            }
        }
        throw "Not enough space";
    }

    std::vector<std::string> split(const std::string & str, const char delimiter) {
        std::stringstream test(str);
        std::string segment;
        std::vector<std::string> seglist;

        while (std::getline(test, segment, delimiter)) {
            seglist.push_back(segment);
        }

        return seglist;
    }
};

#endif // FILE_SYSTEM_H
