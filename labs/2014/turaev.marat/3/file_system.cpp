#include "file_system.h"

#include <math.h>
#include <cstring>
#include <string>

using std::string;

file_system::file_system(const char *root)
: root(root),
config(get_config()),
BLOCK_DATA_SIZE(config.block_size - sizeof (block_descriptor)),
initialized(ifstream(get_block_f(0), ios::binary)) {

    if (initialized) {
        root_d = read_descriptor(0);
        read_block_map();
    }
}

file_system::~file_system() {
    if (initialized) {
        write_block_map();
    }
}

void file_system::format() {
    check_initialized();

    int rd_size = sizeof (file_descriptor); // size of data
    root_d = init_descriptor("/"); // recreate root descriptor
    root_d.id = 0;

    block_map.assign(config.block_number, false); // all blocks are free
    int block_map_blocks = ceil((config.block_number / 8.0 + rd_size) / config.block_size);
    for (int i = 0; i < block_map_blocks; ++i) { // set used blocks
        block_map[i] = true;
    }
}

void file_system::rm(const file_descriptor & fd) {
    if (fd.directory) {
        if (fd.first_child != -1) {
            int next_child = fd.first_child;
            do {
                file_descriptor c = read_descriptor(next_child);
                rm(c);
                next_child = c.next_file;
            } while (next_child != -1);
        }
    } else {
        clear_descriptor(fd);
    }
    remove_descriptor(fd.id);
}

vector<file_descriptor> file_system::list(const file_descriptor & directory) const {
    vector<file_descriptor> fds;
    if (directory.first_child != -1) {
        fds.push_back(read_descriptor(directory.first_child));
        while (fds.back().next_file != -1) {
            fds.push_back(read_descriptor(fds.back().next_file));
        }
    }
    return fds;
}

file_descriptor file_system::init_descriptor(const string & name, bool directory) {
    file_descriptor dd;
    dd.id = -1;
    dd.directory = directory;
    dd.first_child = -1;
    dd.parent_file = -1;
    dd.next_file = -1;
    dd.prev_file = -1;
    dd.first_block = -1;
    dd.number_of_blocks = 0;
    strncpy(dd.name, name.c_str(), sizeof (dd.name));
    dd.time = time(0);
    return dd;
}

void file_system::clear_descriptor(const file_descriptor & fd) {
    if (fd.first_block != -1) {
        block_descriptor bd = read_descriptor<block_descriptor>(fd.first_block);
        while (bd.next != -1) {
            block_map[bd.id] = false;
            bd = read_descriptor<block_descriptor>(bd.next);
        }
        block_map[bd.id] = false;
    }
}

void file_system::remove_descriptor(const int id) {

    file_descriptor fd = read_descriptor(id);

    if (fd.prev_file == -1) { // no previous file
        if (fd.next_file == -1) { // only file in directory
            file_descriptor parent = read_descriptor(fd.parent_file);
            parent.first_child = -1;
            write_descriptor(parent);
        } else { // has next file
            file_descriptor parent = read_descriptor(fd.parent_file);
            file_descriptor next = read_descriptor(fd.next_file);
            parent.first_child = next.id;
            next.parent_file = parent.id;
            next.prev_file = -1;
            write_descriptor(parent);
            write_descriptor(next);
        }
    } else { // has previous file
        if (fd.next_file == -1) { // no next file, is last file
            file_descriptor prev = read_descriptor(fd.prev_file);
            prev.next_file = -1;
            write_descriptor(prev);
        } else { // has next file
            file_descriptor prev = read_descriptor(fd.prev_file);
            file_descriptor next = read_descriptor(fd.next_file);
            prev.next_file = next.id;
            next.prev_file = prev.id;
            write_descriptor(prev);
            write_descriptor(next);
        }
    }

    block_map[fd.id] = false;
}

file_descriptor file_system::find_descriptor(file_descriptor & directory, const string & descriptor_name, bool create, bool is_directory) {
    if (directory.first_child == -1) { // empty directory
        if (create) {
            file_descriptor new_descriptor = init_descriptor(descriptor_name, is_directory);
            directory.first_child = new_descriptor.id = find_first_free_block();
            new_descriptor.parent_file = directory.id;
            write_descriptor(directory);
            write_descriptor(new_descriptor);
            return new_descriptor;
        }
    } else { // directory not empty, looking up children
        file_descriptor current = read_descriptor(directory.first_child);
        while (descriptor_name != current.name && current.next_file != -1) {
            current = read_descriptor(current.next_file);
        }
        if (descriptor_name == current.name) { // descriptor found
            return current;
        } else { // descriptor not found if this directory
            if (create) {
                file_descriptor new_file = init_descriptor(descriptor_name, is_directory);
                current.next_file = new_file.id = find_first_free_block();
                new_file.prev_file = current.id;
                write_descriptor(current);
                write_descriptor(new_file);
                return new_file;
            }
        }
    }
    throw "Cannot find '" + descriptor_name + "'";
}

file_descriptor file_system::find_descriptor(const char *destination, bool create, bool is_directory) {
    vector<string> path = split(string(destination), '/');
    string descriptor_name = path.back();
    path.pop_back();
    if (descriptor_name == "") {
        return root_d;
    }
    file_descriptor directory = find_directory(path, create);
    return find_descriptor(directory, descriptor_name, create, is_directory);
}

file_descriptor file_system::find_directory(const char *destination, bool create) {
    return find_directory(split(destination, '/'), create);
}

file_descriptor file_system::find_directory(const vector<string> & path, bool create) {
    if (path.size() < 1 || path[0] != "") {
        throw "Path should start with /";
    }

    file_descriptor current = root_d; // start with root

    for (auto it = path.begin() + 1; it != path.end(); ++it) {
        if (current.first_child == -1) { // empty directory
            if (create) {
                file_descriptor new_directory = init_descriptor(*it);
                current.first_child = new_directory.id = find_first_free_block();
                new_directory.parent_file = current.id;
                write_descriptor(current);
                write_descriptor(new_directory);
                current = new_directory; // go next level
            } else {
                throw "Cannot find '" + *it + "'";
            }
        } else { // directory not empty, start traversing its children
            current = read_descriptor(current.first_child);
            while (*it != current.name && current.next_file != -1) {
                current = read_descriptor(current.next_file);
            }
            if (*it == current.name) { // descriptor found
                if (!current.directory) {
                    throw "'" + *it + "' is a file";
                }
            } else { // descriptor not found
                if (create) {
                    file_descriptor new_directory = init_descriptor(*it);
                    current.next_file = new_directory.id = find_first_free_block();
                    new_directory.prev_file = current.id;
                    write_descriptor(current);
                    write_descriptor(new_directory);
                    current = new_directory; // go next level
                } else {
                    throw "Cannot find '" + *it + "'";
                }
            }
        }
    }

    return current;
}

void file_system::read_data(const file_descriptor & fd, std::ostream & destination_stream) {
    char * buf = new char[BLOCK_DATA_SIZE];
    int next = fd.first_block;

    while (next != -1) {
        ifstream block(get_block_f(next), ios::binary);
        block_descriptor block_d;
        block.read((char *) &block_d, sizeof (block_descriptor));
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

void file_system::write_data(file_descriptor & fd, std::istream & source_stream) {

    clear_descriptor(fd);

    char * buf = new char[BLOCK_DATA_SIZE];
    size_t id = fd.first_block = find_first_free_block();
    while (source_stream) {
        source_stream.read(buf, BLOCK_DATA_SIZE);

        block_descriptor bd;
        bd.id = id;
        bd.len = source_stream.gcount();
        bd.next = source_stream ? find_first_free_block(id) : -1;
        block_map[bd.id] = true;

        ofstream block(get_block_f(id), ios::binary | ios::trunc);
        block.write((char *) &bd, sizeof (block_descriptor));
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

void file_system::read_block_map() {
    block_map.clear();
    block_map.assign(config.block_number, false);
    int current_block_id = 0;
    ifstream block(get_block_f(current_block_id), ios::in | ios::binary);
    block.seekg(sizeof (file_descriptor));
    for (int i = 0; i < config.block_number;) {
        char byte;
        block >> byte;
        for (int b = 0; b < 8 && i < config.block_number; b++, i++) {
            block_map[i] = byte & (1 << b);
        }
        if (block.tellg() == config.block_size) {
            block.close();
            block.open(get_block_f(++current_block_id), ios::binary);
        }
    }
}

void file_system::write_block_map() {
    int current_block_id = 0;
    ofstream block(get_block_f(current_block_id), ios::binary | ios::trunc);
    block.write((char *) &root_d, sizeof (file_descriptor));
    if (!block) {
        throw "Can't open block";
    }
    block.seekp(sizeof (file_descriptor));
    for (int i = 0; i < config.block_number;) {
        char byte = 0;
        for (int b = 0; b < 8 && i < config.block_number; b++, i++) {
            byte |= block_map[i] << b;
        }
        block << byte;
        if (!block) {
            throw "Write error during writing bitmap";
        }
        if (block.tellp() == config.block_size) {
            block.close();
            block.open(get_block_f(++current_block_id), ios::binary);
            if (!block) {
                throw "Can't open block";
            }
        }
    }
    block.close();
}

void file_system::copy(const file_descriptor &from, file_descriptor &to) {

    if (from.directory && !to.directory) { // dir to file
        throw string("'") + to.name + "' is a file";
    } else if (from.directory && to.directory) { // dir to dir
        if (from.first_child != -1) {
            int next_child = from.first_child;
            do {
                file_descriptor child = read_descriptor(next_child);
                file_descriptor new_d = find_descriptor(to, child.name, true, child.directory);
                copy(child, new_d);
                next_child = child.next_file;
            } while (next_child != -1);
        }
    } else if (!from.directory && to.directory) { //file to dir
        file_descriptor new_file = find_descriptor(to, from.name, true, false);
        copy(from, new_file);
    } else if (!from.directory && !to.directory) { // file to file
        clear_descriptor(to);

        char * buf = new char[BLOCK_DATA_SIZE];
        int next_source = from.first_block;
        int next_dest = to.first_block = find_first_free_block();
        do {
            block_descriptor block_d;
            // read old block
            ifstream block(get_block_f(next_source), ios::binary);
            block.read((char *) &block_d, sizeof (block_descriptor));
            block.read(buf, block_d.len);
            block.close();
            next_source = block_d.next;

            block_descriptor new_block_d = block_d;
            new_block_d.id = next_dest;
            new_block_d.next = next_dest = (next_source == -1 ? -1 : find_first_free_block(new_block_d.id));
            //write new one
            ofstream new_block(get_block_f(new_block_d.id), ios::binary);
            new_block.write((char *) &new_block_d, sizeof (block_descriptor));
            new_block.write(buf, new_block_d.len);
            block_map[new_block_d.id] = true;
        } while (next_source != -1);

        to.number_of_blocks = from.number_of_blocks;
        write_descriptor(to);
    }
}

void file_system::move(const file_descriptor & source_d, file_descriptor & destination_d) {
    if (source_d.directory) {
        if (destination_d.directory) {
            if (source_d.first_child != 1) {
                file_descriptor first_child = read_descriptor(source_d.first_child);
                if (destination_d.first_child == -1) {
                    destination_d.first_child = first_child.id;
                    first_child.parent_file = destination_d.id;
                    write_descriptor(destination_d);
                } else {
                    file_descriptor last_child = read_descriptor(destination_d.first_child);
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
        } else { // cannot move directory to a file
            throw string("'") + destination_d.name + "' is a file";
        }
    } else { // source is a file
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

configuration file_system::get_config() {
    configuration config;
    std::ifstream config_file((string(root) + "/config").c_str());
    if (!config_file.good()) {
        throw "Cannot open config file";
    }
    config_file >> config.block_size >> config.block_number;
    if (!config_file.good()
            || config.block_size < 1024 || config.block_number < 1) {
        throw "Config file is bad";
    }
    return config;
}

