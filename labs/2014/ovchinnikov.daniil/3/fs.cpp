#include "fs.h"
#include "string_utils.h"

#include <math.h>
#include <cstring>

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
    root_d = init_descriptor("/");                      // recreate root descriptor
    root_d.id = 0;

    block_map.assign(config.block_number, false);       // all blocks are free
    int block_map_blocks = ceil((config.block_number / 8.0 + rd_size) / config.block_size);
    for (int i = 0; i < block_map_blocks; ++i) {        // set used blocks
        block_map[i] = true;
    }
}

void FS::rm(const FileDescriptor & fd) {
    if (fd.directory) {
        if (fd.first_child != -1) {
            int next_child = fd.first_child;
            do {
                FileDescriptor c = read_descriptor(next_child);
                rm(c);
                next_child = c.next_file;
            } while (next_child != -1);
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

    if (fd.prev_file == -1) {       // no previous file
        if (fd.next_file == -1) {   // only file in directory
            FileDescriptor parent = read_descriptor(fd.parent_file);
            parent.first_child = -1;
            write_descriptor(parent);
        } else {                    // has next file
            FileDescriptor parent = read_descriptor(fd.parent_file);
            FileDescriptor next = read_descriptor(fd.next_file);
            parent.first_child = next.id;
            next.parent_file = parent.id;
            next.prev_file = -1;
            write_descriptor(parent);
            write_descriptor(next);
        }
    } else {                        // has previous file
        if (fd.next_file == -1) {   // no next file, is last file
            FileDescriptor prev = read_descriptor(fd.prev_file);
            prev.next_file = -1;
            write_descriptor(prev);
        } else {                    // has next file
            FileDescriptor prev = read_descriptor(fd.prev_file);
            FileDescriptor next = read_descriptor(fd.next_file);
            prev.next_file = next.id;
            next.prev_file = prev.id;
            write_descriptor(prev);
            write_descriptor(next);
        }
    }

    block_map[fd.id] = false;
}

FileDescriptor FS::find_descriptor(FileDescriptor & directory, const string & descriptor_name, bool create, bool is_directory) {
    if (directory.first_child == -1) {          // empty directory
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

FileDescriptor FS::find_descriptor(const char *destination,  bool create, bool is_directory) {
    vector<string> path = split(string(destination), '/');
    string descriptor_name = path.back(); path.pop_back();
    if (descriptor_name == "") {
        return root_d;
    }
    FileDescriptor directory = find_directory(path, create);
    return find_descriptor(directory,descriptor_name,create, is_directory);
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
            throw "Cannot read block";
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

void FS::copy(const FileDescriptor &from, FileDescriptor &to) {

    if (from.directory && !to.directory) {          // dir to file
        throw (string("'") + to.name + "' is a file").c_str();
    } else if (from.directory && to.directory) {    // dir to dir
        if (from.first_child != -1) {
            int next_child = from.first_child;
            do {
                FileDescriptor child = read_descriptor(next_child);
                FileDescriptor new_d = find_descriptor(to, child.name, true, child.directory);
                copy(child, new_d);
                next_child = child.next_file;
            } while (next_child != -1);
        }
    } else  if (!from.directory && to.directory) {  //file to dir
        FileDescriptor new_file = find_descriptor(to, from.name, true, false);
        copy(from, new_file);
    } else if (!from.directory && !to.directory) {  // file to file
        clear_descriptor(to);

        char * buf = new char[BLOCK_DATA_SIZE];
        int next_source = from.first_block;
        int next_dest = to.first_block = find_first_free_block();
        do {
            BlockDescriptor block_d;
            // read old block
            ifstream block(get_block_f(next_source), ios::binary);
            block.read((char *) &block_d, sizeof(BlockDescriptor));
            block.read(buf, block_d.len);
            block.close();
            next_source = block_d.next;

            BlockDescriptor new_block_d = block_d;
            new_block_d.id = next_dest;
            new_block_d.next = next_dest = (next_source == -1 ? -1 : find_first_free_block(new_block_d.id));
            //write new one
            ofstream new_block(get_block_f(new_block_d.id), ios::binary);
            new_block.write((char *)&new_block_d, sizeof(BlockDescriptor));
            new_block.write(buf, new_block_d.len);
            block_map[new_block_d.id] = true;
        } while (next_source != -1);

        to.number_of_blocks = from.number_of_blocks;
        write_descriptor(to);
    }
}

void FS::move(const FileDescriptor & source_d, FileDescriptor & destination_d) {
    if (source_d.directory) {
        if (destination_d.directory) {
            if (source_d.first_child != 1) {
                FileDescriptor first_child = read_descriptor(source_d.first_child);
                if (destination_d.first_child == -1) {
                    destination_d.first_child = first_child.id;
                    first_child.parent_file = destination_d.id;
                    write_descriptor(destination_d);
                } else {
                    FileDescriptor last_child = read_descriptor(destination_d.first_child);
                    while (last_child.next_file != -1) {
                        last_child = read_descriptor(last_child.next_file);
                    }
                    last_child.next_file = first_child.id;
                    first_child.prev_file = last_child.id;
                    write_descriptor(last_child);
                }
                write_descriptor(first_child);
            }
            remove_descriptor(source_d.id);
        } else {    // cannot move directory to a file
            throw (string(destination_d.name) + " is a file").c_str();
        }
    } else {        // source is a file
        if (destination_d.directory) {
            destination_d = find_descriptor(destination_d, source_d.name);
        }
        clear_descriptor(destination_d);
        destination_d.number_of_blocks = source_d.number_of_blocks;
        destination_d.first_block = source_d.first_block;
        destination_d.time = time(0);
        write_descriptor(destination_d);
        remove_descriptor(source_d.id);
    }
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
