#ifndef FS_H
#define FS_H

#include "data.h"
#include <vector>
#include <fstream>

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;


class FS {

public:
    FS(const char *root);
    ~FS();

    void format();
    void rm(FileDescriptor &fd);
    vector<FileDescriptor> list(const FileDescriptor & directory) const;

    FileDescriptor init_descriptor(const string &name, bool directory = true);
    void clear_descriptor(const FileDescriptor &fd);
    void remove_descriptor(const int);
    void insert_child(FileDescriptor &d, FileDescriptor f);

    FileDescriptor find_descriptor(const char *destination,  bool create = true, bool is_directory = false);
    FileDescriptor find_directory(const char *destination, bool create = true);
    FileDescriptor find_directory(const vector<string> & path, bool create = true);

    void read_data(const FileDescriptor & fd, std::ostream & destination_stream);
    void write_data(FileDescriptor & fd, std::istream & source_stream);

    Config get_config();

    template <typename D = FileDescriptor>
    D read_descriptor(int id) const {
        D fd;
        ifstream block(get_block_f(id), ios::binary);
        block.read((char *) &fd, sizeof(D));
        if (block) {
            return fd;
        } else {
            throw "Cannot read descriptor ";
        }
    }

    void write_descriptor(const FileDescriptor & d) {
        if (d.id == 0) {
            root_d = d;
        }
        ofstream block(get_block_f(d.id), ios::binary);
        block.write((char *) &d, sizeof(FileDescriptor));
        if (!block) {
            throw "Cannot write descriptor";
        }
        block_map[d.id] = true;
    }

    void check_initialized(){
        if (!initialized) {
            throw "Filesystem isn't initialized yet";
        }
    }

    const char *get_block_f(const int id) const {
        return (string(root) +"/"+ std::to_string(id)).c_str();
    }

private:
    const char *root;
    const Config config;
    const int BLOCK_DATA_SIZE;
    const bool initialized;
    FileDescriptor root_d;
    vector<bool> block_map;

    void read_block_map();
    void write_block_map();

    size_t find_first_free_block(const int start = 0) const {
        for (size_t i = start + 1; i < block_map.size(); i++){
            if (!block_map[i]) {
                return i;
            }
        }
        throw "Not enough space";
    }
};

#endif // FS_H
