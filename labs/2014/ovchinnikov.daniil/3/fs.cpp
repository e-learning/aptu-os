#include "fs.h"
#include "string_utils.h"

#include <math.h>
#include <cstring>
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <sstream>

//#include <unistd.h>

//using std::vector;
//using std::ofstream;
//using std::ifstream;
//using std::ios;
//using std::string;
//using std::istringstream;
//using std::to_string;

FS::FS(const char *root)
    : root(root),
      config(get_config()),
      BLOCK_DATA_SIZE(config.block_size - sizeof(BlockDescriptor)),
      initialized(ifstream(get_block_f(0), ios::binary)) ,
      root_d(read_descriptor(0)){

    if (initialized) {
        read_block_map();
    }
}

FS::~FS() {
    if (initialized) {
        write_block_map();
    }
}

void FS::format() {
    check_initialized();

    int rd_size = sizeof(FileDescriptor);               // size of data
    root_d = init_descriptor("/");                    // recreate root descriptor
    root_d.id = 0;
    block_map.assign(config.block_number, false);       // all blocks are free
    int block_map_blocks = ceil((config.block_number / 8.0 + rd_size) / config.block_size);
    for (int i = 0; i < block_map_blocks; ++i) {        // set used blocks
        block_map[i] = true;
    }
}

void FS::rm(FileDescriptor & fd) {
    if (fd.directory) {
        if (fd.first_child != -1) {
            FileDescriptor c = read_descriptor(fd.first_child);
            while (c.next_file!= -1){
                rm(c);
                c = read_descriptor(c.next_file);
            }
            rm(c);
        }
    } else {
        clear_descriptor(fd);
    }
    remove_descriptor(fd.id);
}

vector<FileDescriptor> FS::list(const FileDescriptor & directory) const {
    vector<FileDescriptor> fds;
    if (directory.first_child != -1) {
        fds.push_back(read_descriptor(directory.first_child));
        while (fds.back().next_file != -1) {
            fds.push_back(read_descriptor(fds.back().next_file));
        }
    }
    return fds;
}

FileDescriptor FS::init_descriptor(const string & name, bool directory) {
    FileDescriptor dd;
    dd.id               = -1;
    dd.directory        = directory;
    dd.first_child      = -1;
    dd.parent_file      = -1;
    dd.next_file        = -1;
    dd.prev_file        = -1;
    dd.first_block      = -1;
    dd.number_of_blocks = 0;
    strncpy(dd.name, name.c_str(), sizeof(dd.name));
    dd.time             = time(0);
    return dd;
}

void FS::clear_descriptor(const FileDescriptor & fd) {
    if (fd.first_block != -1) {
        BlockDescriptor bd = read_descriptor<BlockDescriptor>(fd.first_block);
        while (bd.next != -1) {
            block_map[bd.id] = false;
            bd = read_descriptor<BlockDescriptor>(bd.next);
        }
        block_map[bd.id] = false;
    }
}

void FS::remove_descriptor(const int id) {
    FileDescriptor fd = read_descriptor(id);

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

void FS::insert_child(FileDescriptor & d, FileDescriptor f) {
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

FileDescriptor FS::find_descriptor(const char *destination,  bool create, bool is_directory) {

    vector<string> path = split(string(destination), '/');

    string descriptor_name = path.back(); path.pop_back();
    if (descriptor_name == "") {
        return root_d;
    }

    FileDescriptor directory = find_directory(path, create);

    if (directory.first_child == -1) {      // empty directory
        if (create) {
            FileDescriptor new_descriptor = init_descriptor(descriptor_name, is_directory);
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
            return current;
        } else {                                // descriptor not found if this directory
            if (create) {
                FileDescriptor new_file = init_descriptor(descriptor_name, is_directory);
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

FileDescriptor FS::find_directory(const char *destination, bool create) {
    return find_directory(split(destination, '/'), create);
}

FileDescriptor FS::find_directory(const vector<string> & path, bool create) {
    if (path.size() < 1 || path[0] != "") {
        throw "Path should start with /";
    }

    FileDescriptor current = root_d;                // start with root

    for (auto it = path.begin() + 1; it != path.end(); ++it) {
        if (current.first_child == -1) {            // empty directory
            if (create) {
                FileDescriptor new_directory = init_descriptor(*it);
                current.first_child = new_directory.id = find_first_free_block();
                new_directory.parent_file = current.id;
                write_descriptor(current);
                write_descriptor(new_directory);
                current = new_directory;            // go next level
            } else {
                throw ("Cannot find '" + *it + "'").c_str();
            }
        } else { // directory not empty, start traversing its children
            current = read_descriptor(current.first_child);
            while (*it != current.name && current.next_file != -1) {
                current = read_descriptor(current.next_file);
            }
            if (*it == current.name) {              // descriptor found
                if (!current.directory) {
                    throw ("'" + *it + "' is a file").c_str();
                }
            } else {                                // descriptor not found
                if (create) {
                    FileDescriptor new_directory = init_descriptor(*it);
                    current.next_file = new_directory.id = find_first_free_block();
                    new_directory.prev_file = current.id;
                    write_descriptor(current);
                    write_descriptor(new_directory);
                    current = new_directory;        // go next level
                } else {
                    throw ("Cannot find '" + *it + "'").c_str();
                }
            }
        }
    }

    return current;
}

void FS::read_data(const FileDescriptor & fd, std::ostream & destination_stream) {
    char * buf = new char[BLOCK_DATA_SIZE];
    int next = fd.first_block;

    while (next != -1) {
        ifstream block(get_block_f(next), ios::binary);
        BlockDescriptor block_d;
        block.read((char *) &block_d, sizeof(BlockDescriptor));
        block.read(buf, block_d.len);
        block.close();
        if (!block) {
            delete[] buf;
            throw "Cannot write block";
        }

        destination_stream.write(buf, block_d.len);
        next = block_d.next;
    }
    delete[] buf;
}

void FS::write_data(FileDescriptor & fd, std::istream & source_stream) {

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

void FS::read_block_map() {
    block_map.clear();
    block_map.assign(config.block_number, false);
    int current_block_id = 0;
    ifstream block(get_block_f(current_block_id), ios::in|ios::binary);
    block.seekg(sizeof(FileDescriptor));
    for (int i = 0; i < config.block_number; ){
        char byte;
        block >> byte;
        for (int b = 0; b < 8 && i < config.block_number; b++,i++) {
            block_map[i] = byte & (1 << b);
        }
        if (block.tellg() == config.block_size){
            block.close();
            block.open(get_block_f(++current_block_id), ios::binary);
        }
    }
}

void FS::write_block_map() {
    int current_block_id = 0;
    ofstream block(get_block_f(current_block_id), ios::binary|ios::trunc);
    block.write((char *)&root_d, sizeof(FileDescriptor));
    if (!block) {
        throw "Can't open block";
    }
    block.seekp(sizeof(FileDescriptor));
    for (int i = 0; i < config.block_number;) {
        char byte = 0;
        for (int b = 0; b < 8 && i < config.block_number; b++,i++) {
            byte |= block_map[i] << b;
        }
        block << byte;
        if (!block) {
            throw "Write error during writing bitmap";
        }
        if (block.tellp() == config.block_size){
            block.close();
            block.open(get_block_f(++current_block_id), ios::binary);
            if (!block) {
                throw "Can't open block";
            }
        }
    }
    block.close();
}

Config FS::get_config(){
    Config config;
    std::ifstream config_file((string(root) + "/config").c_str());
    if (!config_file.good()) {
        throw "Cannot open config file";
    }
    config_file  >> config.block_size >> config.block_number;
    if (!config_file.good()
            || config.block_size > 50000000 || config.block_size < 1024
            || config.block_number > 50000 || config.block_number < 1) {
        throw "Config file is bad";
    }
    return config;
}
