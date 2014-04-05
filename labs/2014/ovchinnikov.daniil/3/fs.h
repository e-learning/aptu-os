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

    int init();
    int format();
    int import(const char *filename, const char *destination);
    int mkdir(const char *);

    FileDescriptor find(const string fs_path) {
       return create_directory_if_not_exists(split(fs_path, '/'), false);
    }

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

    int rm(FileDescriptor fd) {
        if (fd.directory) {
            vector<FileDescriptor> files = list(fd);
            if (fd.first_child != -1) {
                FileDescriptor c = read_descriptor(fd.first_child);
                while (c.next_file!= -1){
                    rm(c);
                    c = read_descriptor(c.next_file);
                }
                rm(c);
            }
        } else {
            BlockDescriptor bd = read_descriptor<BlockDescriptor>(fd.first_block);
            while (bd.next != -1) {
                block_map[bd.id] = false;
                bd = read_descriptor<BlockDescriptor>(bd.next);
            }
            block_map[bd.id] = false;
        }

        if (fd.prev_file == -1 && fd.next_file == -1) {
            FileDescriptor parent = read_descriptor(fd.parent_file);
            parent.first_child = -1;
            write_descriptor(parent);
        } else if (fd.prev_file == -1 && fd.next_file != -1) {
            FileDescriptor parent = read_descriptor(fd.parent_file);
            FileDescriptor next = read_descriptor(fd.next_file);
            parent.first_child = next.id;
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

        return 0;
    }

private:
    const char * root;
    FileDescriptor root_d;
    Config config;
    bool initialized;
    vector<bool> block_map;
    const int BLOCK_DATA_SIZE;

    void read_block_map();
    void write_block_map();



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

    FileDescriptor create_directory_if_not_exists(vector<string> path, bool create = true) {
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
                    current = dd;                           // go next level
                } else {
                    throw ("Cannot find " + *it).c_str();
                }
            } else {
                // directory not empty, start traversing its children
                current = read_descriptor(current.first_child);
                while (*it != current.name && current.next_file != -1) {
                    current = read_descriptor(current.next_file);
                }
                if (*it == current.name) {
                    if ((create && !current.directory)
                            || (!create && !current.directory && it + 2 == path.end())) {       // check if it is a directory
                        throw (*it + " is a file").c_str();
                    }
                } else if (current.next_file == -1) {
                    if (create) {
                        // name not found, create new one
                        FileDescriptor dd = create_descriptor(*it);
                        current.next_file = dd.id = find_first_free_block();
                        dd.prev_file = current.id;
                        dd.parent_file = current.parent_file;
                        write_descriptor(current);
                        write_descriptor(dd);
                        current = dd;                       // go next level
                    } else {
                        throw ("Cannot find " + *it).c_str();
                    }
                }
            }
        }
        return current;
    }

    FileDescriptor create_file_for_write(const char * destination) {
        vector<string> path = split(string(destination), '/');
        string file_name = path.back(); path.pop_back();
        FileDescriptor directory = create_directory_if_not_exists(path);

        FileDescriptor nd = create_descriptor(file_name, false);
        if (directory.first_child == -1) {      // empty directory
            directory.first_child = nd.id = find_first_free_block();
            nd.parent_file = directory.id;
            write_descriptor(directory);
        } else {
            FileDescriptor current = read_descriptor(directory.first_child);
            while (file_name != current.name && current.next_file != -1) {
                current = read_descriptor(current.next_file);
            }
            if (file_name == current.name) {
                if (current.directory) {
                    throw (file_name + " is a directory").c_str();
                } else {
                    nd = current;
                    nd.time = time(0);
                    BlockDescriptor bd = read_descriptor<BlockDescriptor>(current.first_block);
                    while (bd.next != -1) {
                        block_map[bd.id] = false;
                        bd = read_descriptor<BlockDescriptor>(bd.next);
                    }
                    block_map[bd.id] = false;
                }
            } else {
                current.next_file = nd.id = find_first_free_block();
                nd.prev_file = current.id;
                nd.parent_file = current.parent_file;
                write_descriptor(current);
            }
        }
        write_descriptor(nd);
        return nd;
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
