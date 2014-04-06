#ifndef FS_H
#define FS_H

#include <vector>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include "data.h"
#include "string_utils.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::vector;
using std::getline;
using std::stringstream;

class FS {

public:
    FS(const char *root);
    ~FS();

    void format();
    void rm(FileDescriptor fd);

    Config get_config();

    vector<FileDescriptor> list(const FileDescriptor & directory) {
        vector<FileDescriptor> fds;
        if (directory.directory) {
            if (directory.first_child != -1) {
                fds.push_back(read_descriptor(directory.first_child));
                while (fds.back().next_file != -1) {
                    fds.push_back(read_descriptor(fds.back().next_file));
                }
            }
        }
        return fds;
    }


    void clear_descriptor(FileDescriptor fd) {
        if (fd.first_block != -1) {
            BlockDescriptor bd = read_descriptor<BlockDescriptor>(fd.first_block);
            while (bd.next != -1) {
                block_map[bd.id] = false;
                bd = read_descriptor<BlockDescriptor>(bd.next);
            }
            block_map[bd.id] = false;
        }
    }

    void remove_descriptor(FileDescriptor & fd) {
        fd = read_descriptor(fd.id);

        if (fd.prev_file == -1 && fd.next_file == -1) {
            FileDescriptor parent = read_descriptor(fd.parent_file);
            parent.first_child = -1;
            write_descriptor(parent);
        } else if (fd.prev_file == -1 && fd.next_file != -1) {
            FileDescriptor parent = read_descriptor(fd.parent_file);
            FileDescriptor next = read_descriptor(fd.next_file);
            parent.first_child = next.id;
            next.parent_file = parent.id;
            next.prev_file = -1;
            write_descriptor(parent);
            write_descriptor(next);
        } else if (fd.prev_file != -1 && fd.next_file == -1) {
            FileDescriptor prev = read_descriptor(fd.prev_file);
            prev.next_file = -1;
            write_descriptor(prev);
        } else {
            FileDescriptor prev = read_descriptor(fd.prev_file);
            FileDescriptor next = read_descriptor(fd.next_file);
            prev.next_file = next.id;
            next.prev_file = prev.id;
            write_descriptor(prev);
            write_descriptor(next);
        }

        block_map[fd.id] = false;
    }

    const char * root;
    FileDescriptor root_d;
    Config config;
    bool initialized;
    vector<bool> block_map;
    const int BLOCK_DATA_SIZE;

    void read_block_map();
    void write_block_map();

    void insert_child(FileDescriptor & d, FileDescriptor & f) {
        d = read_descriptor(d.id);
        f = read_descriptor(f.id);

        if (d.first_child != -1) {
            FileDescriptor c = read_descriptor(d.first_child);
            c.prev_file = f.id;
            f.next_file = c.id;
            c.parent_file = -1;
            write_descriptor(c);
        }

        f.parent_file = d.id;
        d.first_child = f.id;

        write_descriptor(d);
        write_descriptor(f);
    }

    FileDescriptor create_descriptor(const string & name, bool directory = true) {
        FileDescriptor dd;
        dd.id = -1;
        dd.directory        = directory;
        dd.first_child      = -1;
        dd.parent_file      = -1;
        dd.next_file        = -1;
        dd.prev_file        = -1;
        dd.first_block      = -1;
        dd.number_of_blocks = 0;
        strncpy(dd.name, name.c_str(), sizeof(dd.name));
        dd.time = time(0);
        return dd;
    }

    void write_data(FileDescriptor fd, std::istream & source_stream) {

        clear_descriptor(fd);

        char * buf = new char[BLOCK_DATA_SIZE];
        size_t id = fd.first_block = find_first_free_block();
        while (source_stream) {
            source_stream.read(buf, BLOCK_DATA_SIZE);

            BlockDescriptor bd;
            bd.id = id;
            bd.len = source_stream.gcount();
            bd.next = source_stream ? find_first_free_block (id) : -1;
            block_map[bd.id] = true;

            ofstream block(get_block_f(id), ios::binary | ios::trunc);
            block.write((char *) &bd, sizeof(BlockDescriptor));
            block.write(buf, bd.len);
            block.close();
            if (!block) {
                throw "Cannot write block";
            }

            fd.number_of_blocks++;
            id = bd.next;
        }

        write_descriptor(fd);
    }

    FileDescriptor find_directory(vector<string> path, bool create = true) {
        if (path.size() < 1 || path[0] != "") {
            throw "Path should start with /";
        }

        FileDescriptor current = read_descriptor(0);    // start with root

        for (auto it = path.begin() + 1; it != path.end(); ++it) {
            if (current.first_child == -1) {             // empty directory
                if (create) {
                    FileDescriptor dd = create_descriptor(*it);
                    current.first_child = dd.id = find_first_free_block();
                    dd.parent_file = current.id;
                    write_descriptor(current);
                    write_descriptor(dd);
                    current = dd;                       // go next level
                } else {
                    throw ("Cannot find " + *it).c_str();
                }
            } else { // directory not empty, start traversing its children
                current = read_descriptor(current.first_child);
                while (*it != current.name && current.next_file != -1) {
                    current = read_descriptor(current.next_file);
                }
                if (*it == current.name) {              // descriptor found
                    if (!current.directory) {
                        throw (*it + " is a file").c_str();
                    }
                } else {                                // descriptor not found
                    if (create) {
                        FileDescriptor new_directory = create_descriptor(*it);
                        current.next_file = new_directory.id = find_first_free_block();
                        new_directory.prev_file = current.id;
                        write_descriptor(current);
                        write_descriptor(new_directory);
                        current = new_directory;        // go next level
                    } else {
                        throw ("Cannot find " + *it).c_str();
                    }
                }
            }
        }

        return current;
    }

    FileDescriptor find_descriptor(const char *destination,  bool create = true, bool is_directory = false) {

        vector<string> path = split(string(destination), '/');

        string descriptor_name = path.back(); path.pop_back();
        if (descriptor_name == "") {
            return root_d;
        }

        FileDescriptor directory = find_directory(path, create);

        if (directory.first_child == -1) {      // empty directory
            if (create) {
                FileDescriptor new_descriptor = create_descriptor(descriptor_name, is_directory);
                directory.first_child = new_descriptor.id = find_first_free_block();
                new_descriptor.parent_file = directory.id;
                write_descriptor(directory);
                write_descriptor(new_descriptor);
                return new_descriptor;
            }
        } else {                                    // directory not empty, looking up children
            FileDescriptor current = read_descriptor(directory.first_child);
            while (descriptor_name != current.name && current.next_file != -1) {
                current = read_descriptor(current.next_file);
            }
            if (descriptor_name == current.name) {  // descriptor found
//                if (force_type)
//                if (is_directory && !current.directory) {   // need directory, got file
//                    throw ("'" + descriptor_name + "' is a file").c_str();
//                } else if (!is_directory && current.directory) { // need file, got directory
//                    throw ("'" + descriptor_name + "' is a directory").c_str();
//                } else {
//                    current.time = time(0);
//                    clear_descriptor(current);
                    return current;
//                }
            } else {                                // descriptor not found if this directory
                if (create) {
                    FileDescriptor new_file = create_descriptor(descriptor_name, is_directory);
                    current.next_file = new_file.id = find_first_free_block();
                    new_file.prev_file = current.id;
                    write_descriptor(current);
                    write_descriptor(new_file);
                    return new_file;
                }
            }
        }
        throw ("Cannot find '" + descriptor_name + "'").c_str();
    }

    template <typename D = FileDescriptor>
    D read_descriptor(int id) {
        D fd;
        ifstream block(get_block_f(id), ios::binary);
        block.read((char *)&fd, sizeof(D));
        if (block) {
            return fd;
        } else {
            throw "Cannot read descriptor";
        }
    }

    template <typename D = FileDescriptor>
    void write_descriptor(const D & d) {
        if (d.id == 0) {
            root_d = d;
        }
        ofstream block(get_block_f(d.id), ios::binary);
        block.write((char *) &d, sizeof(D));
        if (!block) {
            throw "Cannot write descriptor";
        }
        block_map[d.id] = true;
    }

    size_t find_first_free_block(const int start = 0) const {
        for (size_t i = start + 1; i < block_map.size(); i++){
            if (!block_map[i]) {
                return i;
            }
        }
        throw "Not enough space";
    }

    const char *get_block_f(const size_t id) const {
        return (string(root) +"/"+ std::to_string(id)).c_str();
    }

    void check_initialized(){
        if (!initialized) {
            throw "File system isn't initialized yet";
        }
    }
};


#endif // FS_H
