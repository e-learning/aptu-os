#include "filesystem.h"
#include "filedesc.h"

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <math.h>
#include <stdexcept>

using std::runtime_error;
using std::string;
using std::vector;
using std::ostream;
using std::istream;
using std::ifstream;
using std::ofstream;
using std::stringstream;

const string FileSystem::CONFIG_FILENAME = "/config";

FileSystem::FileSystem(string const & root) :
    root(root),
    config(get_config()),
    is_init(ifstream(to_string(0, root), ios::binary)) {

    block_data_size = config.block_size - sizeof(Block);
    if (is_init) {
        root_desc = read_desc(0, root);
        read_bitmap();
    }


}

FileSystem::~FileSystem() {
    if (is_init) {
        write_bitmap();
    }

}

Config FileSystem::get_config() {
    Config config;
    ifstream f((root + CONFIG_FILENAME).c_str());
    if (!f.good()) {
        throw runtime_error("Cannot open config file");
    }
    f  >> config.block_size >> config.block_count;
    if (!f.good() || config.block_size < 1024 || config.block_count < 1) {
        throw runtime_error("Error of config file");
    }
    return config;
}

void FileSystem::read_bitmap() {
    bitmap.clear();
    bitmap.assign(config.block_count, false);
    int cur_block = 0;
    ifstream block(to_string(cur_block, root), ios::in|ios::binary);
    block.seekg(sizeof(FileDesc));
    int cur_block_num = 0;
    while (cur_block_num < config.block_count) {
        char byte ;
        block >> byte;
        for (int i = 0; i < 8 && cur_block_num < config.block_count; ++i, ++cur_block_num) {
            bitmap[cur_block_num] = byte & (1 << i);
        }

        if (block.tellg() == config.block_size) {
            block.close();
            block.open(to_string(++cur_block, root), ios::binary);
        }
    }
}

void FileSystem::write_bitmap() {
    int cur_block = 0;
    ofstream block(to_string(cur_block, root), ios::binary|ios::trunc);
    block.write((char *)&root_desc, sizeof(FileDesc));
    if (!block) {
        throw runtime_error("Cannot open block");
    }
    block.seekp(sizeof(FileDesc));
    int cur_block_num = 0;
    while(cur_block_num < config.block_count) {
        char byte = 0;
        for (int i = 0; i < 8 && cur_block_num < config.block_count; ++i, ++cur_block_num) {
            byte |= bitmap[cur_block_num] << i;
        }
        block << byte;
        if (!block) {
            throw runtime_error("Error of writing bitmap");
        }
        if (block.tellp() == config.block_size) {
            block.close();
            block.open(to_string(++cur_block, root), ios::binary);
            if (!block) {
                throw runtime_error("Cannot open block");
            }
        }
    }
    block.close();
}

vector<string> FileSystem::get_path(string const & name) {
    stringstream path(name);
    string dir;
    vector<string> dir_list;

    while(std::getline(path, dir, '/')) {
       dir_list.push_back(dir);
    }

    return dir_list;
}


FileDesc FileSystem::find_desc(FileDesc &dir, const std::string &name_desc, bool create, bool is_dir) {
    if (dir.first_file_id == -1) {
        if (create) {
            FileDesc new_desc = FileDesc(name_desc, is_dir);
            new_desc.id = find_first_free_block();
            dir.first_file_id = new_desc.id;
            new_desc.parent_id = dir.id;
            write_desc(dir, root, bitmap, root_desc);
            write_desc(new_desc, root, bitmap, root_desc);
            return new_desc;
        }
    } else {
        FileDesc cur_desc = read_desc(dir.first_file_id, root);
        while (name_desc != cur_desc.name && cur_desc.next_file_id != -1) {
            cur_desc = read_desc(cur_desc.next_file_id, root);
        }
        if (name_desc == cur_desc.name) {
            return cur_desc;
        } else {
            if (create) {
                FileDesc new_file = FileDesc(name_desc, is_dir);
                new_file.id = find_first_free_block();
                cur_desc.next_file_id = new_file.id;
                new_file.prev_file_id = cur_desc.id;
                write_desc(cur_desc, root, bitmap, root_desc);
                write_desc(new_file, root, bitmap, root_desc);
                return new_file;
            }
        }
    }
    throw runtime_error("Cannot find " + name_desc);
}



FileDesc FileSystem::find_desc(string const & dest, bool create, bool is_dir) {
    vector<string> path = get_path(dest);
    string name_desc = path.back();
    path.pop_back();
    if (name_desc == "") {
        return root_desc;
    } else {
        FileDesc dir = find_dir(path, create);
        return find_desc(dir, name_desc, create, is_dir);
    }

}


void FileSystem::read_data(FileDesc const & fd, ostream & os) {
    char * buffer = new char[block_data_size];
    int next_block = fd.first_block;
    while (next_block != -1) {
        ifstream block_stream(to_string(next_block, root), ios::binary);
        Block block;
        block_stream.read((char*)&block, sizeof(Block));
        block_stream.read(buffer, block.length);
        block_stream.close();
        if (!block_stream) {
            delete[] buffer;
            throw runtime_error("Error read block");
        }
        os.write(buffer, block.length);
        next_block = block.next_id;
    }
   delete[] buffer;
}



void FileSystem::write_data(FileDesc & fd, istream & is) {

    fd.clear(bitmap, root);
    char * buffer = new char[block_data_size];
    fd.first_block = find_first_free_block();
    size_t id = fd.first_block;
    while (is) {
        is.read(buffer, block_data_size);
        Block bd;
        bd.id = id;
        bd.length = is.gcount();
        if (is) {
            bd.next_id = find_first_free_block(id);
        } else {
            bd.next_id = -1;
        }
        bitmap[bd.id] = true;
        ofstream block_stream(to_string(id, root), ios::binary | ios::trunc);
        block_stream.write((char *) &bd, sizeof(Block));
        block_stream.write(buffer, bd.length);
        block_stream.close();
        if (!block_stream) {
            throw runtime_error("Error write block");
        }

        ++fd.block_count;
        id = bd.next_id;
    }

    write_desc(fd, root, bitmap, root_desc);
}


FileDesc FileSystem::find_dir(const vector<std::string> &path, bool create) {
    if (path.size() < 1 || path[0] != "") {
        throw runtime_error("Error Path");
    }

    FileDesc cur_desc = root_desc;
    for (auto it = path.begin() + 1; it != path.end(); ++it) {
        if (cur_desc.first_file_id == -1) {
            if (create) {
                FileDesc new_dir = FileDesc(*it, true);



                new_dir.id = find_first_free_block();
                cur_desc.first_file_id = new_dir.id;
                new_dir.parent_id = cur_desc.id;

                write_desc(cur_desc, root, bitmap, root_desc);
                write_desc(new_dir, root, bitmap, root_desc);
                cur_desc = new_dir;
            } else {
                throw runtime_error("Cannot find " + *it);
            }
        } else {
            cur_desc = read_desc(cur_desc.first_file_id, root);
            while (*it != cur_desc.name && cur_desc.next_file_id != -1) {
                cur_desc = read_desc(cur_desc.next_file_id, root);
            }
            if (*it == cur_desc.name) {
                if (!cur_desc.is_directory) {
                    throw  runtime_error(*it + " is a file");
                }
            } else {
                if (create) {
                    FileDesc new_dir = FileDesc(*it, true);
                    new_dir.id = find_first_free_block();
                    cur_desc.next_file_id = new_dir.id;
                    new_dir.prev_file_id = cur_desc.id;
                    write_desc(cur_desc, root, bitmap, root_desc);
                    write_desc(new_dir, root, bitmap, root_desc);
                    cur_desc = new_dir;
                } else {
                    throw runtime_error("Cannot find " + *it);
                }
            }
        }
    }

    return cur_desc;
}




void FileSystem::init_cmd() {

    for (int i = 0; i < config.block_count; ++i) {
        const string block_name = to_string(i, root);
        ofstream(block_name, ios::trunc).close();
        if (truncate(block_name.c_str(), config.block_size)) {
            throw runtime_error("Cannot create blocks");
        }
    }

}

void FileSystem::format_cmd() {

    if(!is_init) {
        throw runtime_error("Don't initialized");
    }
    root_desc = FileDesc("/",true);
    root_desc.id = 0;
    bitmap.assign(config.block_count, false);
    int bitmap_blocks_count = ceil((config.block_count / 8.0 + sizeof(FileDesc)) / config.block_size);
    for (int i = 0; i < bitmap_blocks_count; ++i) {
        bitmap[i] = true;
    }
}

vector<FileDesc> FileSystem::get_list_child(FileDesc const & dir) {
    vector<FileDesc> fd_list;
    if (dir.first_file_id != -1) {
        fd_list.push_back(read_desc(dir.first_file_id, root));
        while (fd_list.back().next_file_id != -1) {
            fd_list.push_back(read_desc(fd_list.back().next_file_id, root));
        }
    }
    return fd_list;
}

void FileSystem::ls_cmd(const std::string &file_name) {

    FileDesc fd = find_desc(file_name, false, false);
    if (fd.is_directory) {
        vector<FileDesc> v = get_list_child(fd);
        for (auto it = v.begin(); it != v.end(); ++it) {
            std::cout<<(*it).get_info()<<std::endl;
        }

    } else {
        std::cout<<fd.get_info()<<std::endl;
    }
}

void FileSystem::rm(FileDesc & fd) {

    if (fd.is_directory) {
        if (fd.first_file_id != -1) {
            int next_file = fd.first_file_id;
            do {
                FileDesc cur_desc = read_desc(next_file, root);
                rm(cur_desc);
                next_file = cur_desc.next_file_id;
            } while (next_file != -1);
        }
    } else {
        fd.clear(bitmap, root);
    }
    fd.remove(bitmap, root, root_desc);
}

void FileSystem::rm_cmd(const std::string &file_name) {

    FileDesc fd = find_desc(file_name, false, false);
    rm(fd);
}


void FileSystem::mkdir_cmd(const std::string & dir_name) {
    find_dir(dir_name,true);
}

void FileSystem::import_cmd(const std::string &source_file_name, const std::string &target_file_name) {

    if (!is_init) {
        throw runtime_error("Don't initialize");
    }
    ifstream sourse_file(source_file_name, ios::binary);
    if (!sourse_file) {
        throw runtime_error("Error open sourse file");
    }
    FileDesc fd = find_desc(target_file_name,true, false);
    if (fd.is_directory) {
        throw runtime_error("Target file is directory");
    }
    write_data(fd, sourse_file);
    sourse_file.close();
}

void FileSystem::export_cmd(const std::string &source_file_name, const std::string &target_file_name) {
    FileDesc fd = find_desc(source_file_name, false, false);
    ofstream target_file(target_file_name, ios::binary | ios::trunc);
    if (!target_file) {
        throw runtime_error("Error open target file");
    }
    read_data(fd, target_file);
    target_file.close();
}

void FileSystem::copy_cmd(const std::string &source_file_name, const std::string &target_dir_name) {

    FileDesc source_desc = find_desc(source_file_name, false, false);
    FileDesc target_desc = find_desc(target_dir_name, true, source_desc.is_directory);

    copy(source_desc, target_desc);
}

void FileSystem::copy(const FileDesc & source_desc, FileDesc & target_desc) {

    if (source_desc.is_directory && !target_desc.is_directory) {
        throw runtime_error("Target is a file");
    } else if (source_desc.is_directory && target_desc.is_directory) {
        if (source_desc.first_file_id != -1) {
            int next_file = source_desc.first_file_id;
            do {
                FileDesc child = read_desc(next_file, root);
                FileDesc new_desc = find_desc(target_desc, child.name, true, child.is_directory);
                copy(child, new_desc);
                next_file = child.next_file_id;
            } while (next_file != -1);
        }
    } else  if (!source_desc.is_directory && target_desc.is_directory) {
        FileDesc new_file = find_desc(target_desc, source_desc.name, true, false);
        copy(source_desc, new_file);
    } else if (!source_desc.is_directory && !target_desc.is_directory) {
        target_desc.clear(bitmap, root);
        char * buffer = new char[block_data_size];
        int next_source = source_desc.first_block;
        target_desc.first_block = find_first_free_block();
        int next_dest = target_desc.first_block;
        do {
            Block block_desc;
            ifstream block(to_string(next_source, root), ios::binary);
            block.read((char *) &block_desc, sizeof(Block));
            block.read(buffer, block_desc.length);
            block.close();
            next_source = block_desc.next_id;
            Block new_block_desc = block_desc;
            new_block_desc.id = next_dest;
            next_dest = (next_source == -1 ? -1 : find_first_free_block(new_block_desc.id));
            new_block_desc.next_id = next_dest;
            ofstream new_block(to_string(new_block_desc.id, root), ios::binary);
            new_block.write((char *)&new_block_desc, sizeof(Block));
            new_block.write(buffer, new_block_desc.length);
            bitmap[new_block_desc.id] = true;
        } while (next_source != -1);

        target_desc.block_count = source_desc.block_count;
        write_desc(target_desc, root, bitmap, root_desc);
    }
}


void FileSystem::move_cmd(const std::string &source_file_name, const std::string &target_file_name) {
    if (source_file_name != target_file_name) {
        FileDesc source_desc = find_desc(source_file_name, false, false);
        FileDesc target_desc;
        if (source_desc.is_directory) {

            target_desc = find_dir(target_file_name, true);


        } else {
            target_desc = find_desc(target_file_name, true, false);
        }
        move(source_desc, target_desc);
    }
}

void FileSystem::move(FileDesc & source_desc, FileDesc & target_desc) {

    if (source_desc.is_directory) {
        if (target_desc.is_directory) {

            if (source_desc.first_file_id != 1) {
                FileDesc new_target_desc = find_desc(target_desc, source_desc.name, true, true);

                if (source_desc.first_file_id != -1) {
                    FileDesc first_file = read_desc(source_desc.first_file_id, root);
                    if (new_target_desc.first_file_id == -1) {
                        new_target_desc.first_file_id = first_file.id;
                        first_file.parent_id = new_target_desc.id;
                        write_desc(new_target_desc, root, bitmap, root_desc);
                    } else {
                        FileDesc last_child = read_desc(new_target_desc.first_file_id, root);
                        while (last_child.next_file_id != -1) {
                            last_child = read_desc(last_child.next_file_id, root);
                        }
                        last_child.next_file_id = first_file.id;
                        first_file.prev_file_id = last_child.id;
                        write_desc(last_child, root, bitmap, root_desc);
                    }
                    write_desc(first_file, root, bitmap, root_desc);
                } else {

                    write_desc(new_target_desc, root, bitmap, root_desc);
                    write_desc(target_desc, root, bitmap, root_desc);
                }

            }
            FileDesc new_source_desc = read_desc(source_desc.id, root);
            new_source_desc.remove(bitmap, root, root_desc);

        } else {
            throw runtime_error("Target is file");
        }
    } else {
        if (target_desc.is_directory) {
            target_desc = find_desc(target_desc, source_desc.name, true, false);
            target_desc.clear(bitmap, root);
            target_desc.block_count = source_desc.block_count;
            target_desc.first_block = source_desc.first_block;
            target_desc.modified_time = time(0);
            write_desc(target_desc, root, bitmap, root_desc);
            source_desc.remove(bitmap, root, root_desc);
        } else {
            if (target_desc.name != source_desc.name) {
                strncpy(source_desc.name, target_desc.name, sizeof(source_desc.name));
                source_desc.modified_time = time(0);
                write_desc(source_desc, root, bitmap, root_desc);
                target_desc.remove(bitmap, root, root_desc);
            }
        }

    }
}
