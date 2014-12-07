#include "file_system.h"


#include <math.h>
#include <cstring>
#include <fstream>
#include <stdexcept>

using std::string;

file_system::file_system(std::string root)
        : _root(root),
          config(read_config())
{
    if (_root[_root.size() - 1] == '/')
    {
        _root = _root.substr(0, _root.size() - 1);
    }
    if (!std::ifstream(get_block_name(0)).good())
    {
        initialized = false;
    } else
    {
        initialized = true;
    }
}

file_system::~file_system()
{
}

void file_system::file_export(std::string fs_file, std::string host_file)
{
    if (!initialized)
    {throw "Not initialized";}
    read_metainformation();

    file_descriptor file = get_file(fs_file, false, true);
    block *file_block = new block(file.first_block, config.block_size, _root);
    read_data(&file, sizeof(file_descriptor), file_block);

    char *buffer = new char[file.size];
    read_data(buffer, file.size, file_block);

    std::ofstream File(host_file, std::ios::out | std::ios::binary);
    File.write(buffer, file.size);
    if (File.fail()) throw ("Error while saving file.");
    write_metainformation();
}

void file_system::init()
{
    for (int i = 0; i < config.block_no; ++i)
    {
        const string block_name = get_block_name(i);
        std::ofstream out(block_name, std::ios::trunc);
        out.seekp(config.block_size - 1);
        out.write("", 1);
        if (out.bad())
        {
            throw std::runtime_error("Can't allocate blocks");
        }
        out.close();
    }
}

void file_system::import(std::string host_file, std::string filesystem_file)
{
    if (!initialized)
    {throw "Not initialized";}
    read_metainformation();

    if (filesystem_file[filesystem_file.size() - 1] == '/')
    {
        filesystem_file = filesystem_file.substr(0, filesystem_file.size() - 1);
    }
    int last_match = filesystem_file.find_last_of("/");
    std::string fs_file_path = filesystem_file.substr(0, last_match + 1);
    std::string fs_file_name = filesystem_file.substr(last_match + 1);

    file_descriptor dst_fold = get_file(fs_file_path, false, false);
    block *dst_block = new block(dst_fold.first_block, config.block_size, _root);
    for (auto it = dir_iterator(*this, dst_fold); it != dir_iterator(*this); ++it)
    {
        file_descriptor file = *it;
        if (std::string(file.filename) == fs_file_name) throw std::runtime_error("File already exist");
    }

    file_descriptor file;
    block *file_block = get_free_block();
    std::ifstream File(host_file, std::ios::in | std::ios::binary);
    if (File.fail()) throw std::runtime_error("Can't open " + host_file);
    File.seekg(0, std::ios::end);
    int fileLen = File.tellg();
    file.size = fileLen;
    File.seekg(0, std::ios::beg);
    char *data = new char[fileLen];
    File.read(data, fileLen);
    if (File.fail()) throw std::runtime_error("Can't read file " + host_file);
    File.close();

    write_data(&file, sizeof(file_descriptor), file_block);
    write_data(data, fileLen, file_block);

    file.directory = false;
    file.set_filename(fs_file_name);

    file.first_block = file_block->get_index();
    file.parent_file = dst_fold.first_block;
    file.prev_file = -1;
    file.next_file = -1;
    if (dst_fold.first_child != -1)
    {
        block *nb = new block(dst_fold.first_child, config.block_size, _root);
        file_descriptor nd;
        read_data(&nd, sizeof(file_descriptor), nb);
        nd.prev_file = file.first_block;
        update_descriptor(nd, nb);
        file.next_file = nd.first_block;
    }
    dst_fold.first_child = file.first_block;
    update_descriptor(dst_fold, dst_block);
    update_descriptor(file, file_block);
    write_metainformation();
}

void file_system::format()
{
    if (!initialized)
    {throw "Not initialized";}
    meta.block_map_size = config.block_no / 8 + 1;
    meta.block_map = new char[meta.block_map_size];
    memset(meta.block_map, 0, sizeof(char) * meta.block_map_size);
    int free_space_size = config.block_size - sizeof(block_descriptor);
    int no_of_blocks = (sizeof(meta) + meta.block_map_size) / free_space_size + 1;
    meta.root_block = no_of_blocks;
    for (int i = 0; i < no_of_blocks; ++i) reserve_block(i);

    file_descriptor root;
    root.set_filename("/");
    root.first_block = meta.root_block;
    write_data(&root, sizeof(file_descriptor), 0);

    write_metainformation();
}


void file_system::copy(std::string src, std::string dest)
{
    if (!initialized)
    {throw "Not initialized";}
    read_metainformation();
    file_descriptor src_file = get_file(src, false, true);

    if (dest != "/" && dest[dest.size() - 1] == '/')
    {
        dest = dest.substr(0, dest.size() - 1);
    }
    int last_match = dest.find_last_of("/");
    std::string dest_path = dest.substr(0, last_match + 1);
    std::string dest_filename = dest.substr(last_match + 1);

    file_descriptor dst_fold = get_file(dest_path, false, false);
    copy(src_file, dst_fold, dest_filename);
    write_metainformation();
}

void file_system::copy(file_descriptor src_file, file_descriptor &dst_fold, std::string dst_filename)
{
    block *src_block = new block(src_file.first_block, config.block_size, _root);
    read_data(&src_file, sizeof(file_descriptor), src_block);
    char *data = new char[src_file.size];
    read_data(data, src_file.size, src_block);

    file_descriptor file;
    block *file_block = get_free_block();
    write_data(&file, sizeof(file_descriptor), file_block);
    write_data(data, src_file.size, file_block);

    block *dst_block = new block(dst_fold.first_block, config.block_size, _root);


    bool found = false;
    for (auto it = dir_iterator(*this, dst_fold); it != dir_iterator(*this); ++it)
    {
        file_descriptor f = *it;
        if (std::string(f.filename) == dst_filename)
        {
            if (f.directory)
            {
                found = true;
                dst_fold = f;
                dst_block = new block(dst_fold.first_block, config.block_size, _root);
            } else
            {
                throw std::runtime_error("File already exist");
            }
        }
    }

    file.first_block = file_block->get_index();
    if (!found)
    {
        file.set_filename(dst_filename);
    } else
    {
        file.set_filename(src_file.filename);
    }
    file.parent_file = dst_fold.first_block;
    file.size = src_file.size;
    file.directory = src_file.directory;

    if (dst_fold.first_child != -1)
    {
        block *nb = new block(dst_fold.first_child, config.block_size, _root);
        file_descriptor nd;
        read_data(&nd, sizeof(file_descriptor), nb);
        nd.prev_file = file.first_block;
        update_descriptor(nd, nb);
        file.next_file = nd.first_block;
    }
    dst_fold.first_child = file.first_block;
    update_descriptor(dst_fold, dst_block);
    update_descriptor(file, file_block);

    if (src_file.directory)
    {
        for (auto it = dir_iterator(*this, src_file); it != dir_iterator(*this); ++it)
        {
            file_descriptor f = *it;
            copy(f, file, f.filename);
        }
    }
}

void file_system::ls(std::string path)
{
    if (!initialized)
    {throw "Not initialized";}
    read_metainformation();
    file_descriptor file = get_file(path, false, true);

    if (file.directory)
    {
        for (auto it = dir_iterator(*this, file); it != dir_iterator(*this); ++it)
        {
            file_descriptor f = *it;
            std::cout << std::string(f.filename) ;
            if(f.directory){
                std::cout<<" d"<< std::endl;
            }
            else{
                std::cout<<" f" << std::endl;
            }
        }
    }
    else{
        std::cout << file.filename << ' ';
        std::cout << file.size << ' ';
    }
}

void file_system::rm(std::string path)
{
    if (!initialized)
    {throw "Not initialized";}
    read_metainformation();
    rm(get_file(path, false, true));
    write_metainformation();
}

void file_system::rm(file_descriptor file)
{

    if (file.directory)
    {
        for (auto it = dir_iterator(*this, file); it != dir_iterator(*this); ++it)
        {
            rm(*it);
        }
        free_block(file.first_block);
    } else
    {
        int size = file.size + sizeof(file);
        block *curr_block = new block(file.first_block, config.block_size, _root);
        while (size > 0)
        {
            size -= curr_block->_descriptor.data_written;
            free_block(curr_block->get_index());
            if (size > 0)
            {
                curr_block = new block(curr_block->_descriptor.next, config.block_size, _root);
            }
        }
    }

    block *parent_block = new block(file.parent_file, config.block_size, _root);
    file_descriptor parent_descriptor;
    read_data(&parent_descriptor, sizeof(file_descriptor), parent_block);
    if (parent_descriptor.first_child == file.first_block)
    {
        parent_descriptor.first_child = file.next_file;
        update_descriptor(parent_descriptor, parent_block);
    }

    if (file.prev_file != -1)
    {
        block *left_file_block = new block(file.prev_file, config.block_size, _root);
        file_descriptor left_file_descriptor;
        read_data(&left_file_descriptor, sizeof(file_descriptor), left_file_block);
        left_file_descriptor.next_file = file.next_file;
        update_descriptor(left_file_descriptor, left_file_block);
    }

    if (file.next_file != -1)
    {
        block *right_file_block = new block(file.next_file, config.block_size, _root);
        file_descriptor right_file_descriptor;
        read_data(&right_file_descriptor, sizeof(file_descriptor), right_file_block);
        right_file_descriptor.prev_file = file.prev_file;
        update_descriptor(right_file_descriptor, right_file_block);
    }
}


void file_system::mkdir(std::string path)
{
    if (!initialized)
    {throw "Not initialized";}
    read_metainformation();
    get_file(path, true, false);
    write_metainformation();
}

void file_system::move(std::string src, std::string dest)
{

    if (!initialized)
    {throw "Not initialized";}
    read_metainformation();
    file_descriptor src_file = get_file(src, false, true);
    block *src_block = new block(src_file.first_block, config.block_size, _root);

    block *parent_block = new block(src_file.parent_file, config.block_size, _root);
    file_descriptor parent_descriptor;
    read_data(&parent_descriptor, sizeof(file_descriptor), parent_block);
    if (parent_descriptor.first_child == src_file.first_block)
    {
        parent_descriptor.first_child = src_file.next_file;
        update_descriptor(parent_descriptor, parent_block);
    }

    if (src_file.prev_file != -1)
    {
        block *left_file_block = new block(src_file.prev_file, config.block_size, _root);
        file_descriptor left_file_descriptor;
        read_data(&left_file_descriptor, sizeof(file_descriptor), left_file_block);
        left_file_descriptor.next_file = src_file.next_file;
        update_descriptor(left_file_descriptor, left_file_block);
    }

    if (src_file.next_file != -1)
    {
        block *right_file_block = new block(src_file.next_file, config.block_size, _root);
        file_descriptor right_file_descriptor;
        read_data(&right_file_descriptor, sizeof(file_descriptor), right_file_block);
        right_file_descriptor.prev_file = src_file.prev_file;
        update_descriptor(right_file_descriptor, right_file_block);
    }

    if (dest != "/" && dest[dest.size() - 1] == '/')
    {
        dest = dest.substr(0, dest.size() - 1);
    }
    int last_match = dest.find_last_of("/");
    std::string dest_path = dest.substr(0, last_match + 1);
    std::string dest_filename = dest.substr(last_match + 1);

    file_descriptor dst_fold = get_file(dest_path, false, false);

    block *dst_block = new block(dst_fold.first_block, config.block_size, _root);


    bool found = false;
    for (auto it = dir_iterator(*this, dst_fold); it != dir_iterator(*this); ++it)
    {
        file_descriptor f = *it;
        if (std::string(f.filename) == dest_filename)
        {
            if (f.directory)
            {
                found = true;
                dst_fold = f;
                dst_block = new block(dst_fold.first_block, config.block_size, _root);
            } else
            {
                throw std::runtime_error("File already exist");
            }
        }
    }

    if (!found)
    {
        src_file.set_filename(dest_filename);
    }
    src_file.parent_file = dst_fold.first_block;

    if (dst_fold.first_child != -1)
    {
        block *nb = new block(dst_fold.first_child, config.block_size, _root);
        file_descriptor nd;
        read_data(&nd, sizeof(file_descriptor), nb);
        nd.prev_file = src_file.first_block;
        update_descriptor(nd, nb);
        src_file.next_file = nd.first_block;
    }
    dst_fold.first_child = src_file.first_block;
    update_descriptor(dst_fold, dst_block);
    update_descriptor(src_file, src_block);

    write_metainformation();
}

file_descriptor file_system::get_file(std::string path, bool create, bool file_available)
{

    if (path[path.size() - 1] != '/')
    {
        path.push_back('/');
    }

    int pos;
    pos = path.find('/');
    if (pos != 0)
    {throw std::runtime_error("Bad path");}
    path.erase(0, 1);

    file_descriptor curr_dir;
    block *curr_dir_block = new block(meta.root_block, config.block_size, _root);
    read_data(&curr_dir, sizeof(curr_dir), curr_dir_block);

    while (path.size() != 0)
    {
        pos = path.find('/');
        std::string dir_name = path.substr(0, pos);

        bool found = false;
        for (auto it = dir_iterator(*this, curr_dir); it != dir_iterator(*this); ++it)
        {
            file_descriptor file = *it;
            if (std::string(file.filename) == dir_name)
            {
                found = true;
                if (!file.directory)
                {
                    if (file_available && pos == (int) path.size() - 1)
                    {
                        return file;
                    } else
                    {
                        throw std::runtime_error("File in path");
                    }
                }
                curr_dir = file;
            }
        }
        if (!found)
        {
            if (create)
            {
                block *dir_block = get_free_block();
                file_descriptor dir;
                dir.set_filename(dir_name);
                dir.first_block = dir_block->get_index();
                dir.parent_file = curr_dir.first_block;
                if (curr_dir.first_child == -1)
                {
                    curr_dir.first_child = dir.first_block;
                } else
                {
                    block *neighbour_file_block = new block(curr_dir.first_child, config.block_size, _root);
                    curr_dir.first_child = dir.first_block;
                    file_descriptor neighbour_file_descriptor;
                    read_data(&neighbour_file_descriptor, sizeof(file_descriptor), neighbour_file_block);
                    dir.next_file = neighbour_file_descriptor.first_block;
                    neighbour_file_descriptor.prev_file = dir.first_block;
                    update_descriptor(neighbour_file_descriptor, neighbour_file_block);
                }
                update_descriptor(curr_dir, curr_dir_block);
                curr_dir = dir;

                write_data(&dir, sizeof(file_descriptor), dir_block);
            } else
            {
                throw std::runtime_error("No such file or directory");
            }
        }

        curr_dir_block = new block(curr_dir.first_block, config.block_size, _root);
        path.erase(0, pos + 1);
    }
    return curr_dir;
}

filesystem_config file_system::read_config()
{
    filesystem_config config;
    std::ifstream config_file;
    config_file.open(string(_root) + "/config");
    if (!config_file.good())
    {
        throw std::runtime_error("Can't open config file");
    }
    config_file >> config.block_size >> config.block_no;
    if (!config_file.good()
            || config.block_size < 1024
            || config.block_no < 1
            || config.block_size * config.block_no > 50 * 1024 * 1024)
    {
        throw std::runtime_error("Bad config file");
    }
    return config;
}

void file_system::reserve_block(int n)
{
    int pos = n / 8;
    int off = n % 8;
    meta.block_map[pos] |= 1 << off;
}

void file_system::free_block(int n)
{
    int pos = n / 8;
    int off = n % 8;
    meta.block_map[pos] ^= 1 << off;
}

std::string file_system::get_block_name(int block_id)
{
    return _root + "/" + std::to_string(block_id);
}

void file_system::write_metainformation()
{
    int ind = 0;
    int meta_size = sizeof(meta);
    int size = meta.block_map_size;
    block *curr_block = new block(ind, config.block_size);

    char *ptr = meta.block_map;
    curr_block->write(&meta, meta_size);
    while (size != 0)
    {
        int written = curr_block->write(ptr, size);
        if (written == 0) throw std::runtime_error("Some error during writing data occured");
        size -= written;
        ptr += written;
        if (size != 0)
        {
            ++ind;
            curr_block->_descriptor.next = ind;
            curr_block->save_block(_root);
            curr_block = new block(ind, config.block_size);
        } else
        {
            curr_block->save_block(_root);
        }
    }
}


void file_system::read_metainformation()
{
    block *curr_block = new block(0, config.block_size, _root);
    curr_block->read(&meta, sizeof(meta));

    meta.block_map = new char[meta.block_map_size];
    read_data(meta.block_map, meta.block_map_size, curr_block);
}

void file_system::read_data(void *data, int size, block *curr_block)
{
    char *dt = (char *) data;
    while (size != 0)
    {
        int readen = curr_block->read(dt, size);
        if (readen == 0) throw std::runtime_error("Unexpected end of block №" + std::to_string(curr_block->get_index()));
        size -= readen;
        dt += readen;
        if (size != 0)
        {
            curr_block = new block(curr_block->_descriptor.next, config.block_size, _root);
        }
    }
}

void file_system::write_data(void *data, int size, block *first_block = 0)
{
    char *dt = (char *) data;
    block *last_block = 0;
    if (first_block == 0) first_block = get_free_block();
    last_block = first_block;
    block *curr_block = last_block;
    while (size != 0)
    {
        int written = curr_block->write(dt, size);
        size -= written;
        dt += written;
        if (size != 0)
        {
            last_block = get_free_block();
            curr_block->_descriptor.next = last_block->get_index();
            curr_block->save_block(_root);
            curr_block = last_block;
        } else
        {
            curr_block->save_block(_root);
        }
    }
}

block *file_system::get_free_block()
{
    for (int i = 0; i < config.block_no; i++)
    {
        int pos = i / 8;
        int off = i % 8;
        if ((meta.block_map[pos] & (1 << off)) == 0)
        {
            meta.block_map[pos] |= 1 << off;
            return new block(i, config.block_size);
        }
    }
    throw std::runtime_error("Out of memory");
}

dir_iterator::dir_iterator(file_system &fs, file_descriptor dir) :
        _fs(fs)
{
    if (!dir.directory) throw std::runtime_error("Can't iterate by file " + std::string(dir.filename));
    if (dir.first_child > 0 && dir.first_child < _fs.config.block_no)
    {
        p = new file_descriptor;
        block *_block = new block(dir.first_child, _fs.config.block_size, _fs._root);
        _fs.read_data(p, sizeof(file_descriptor), _block);
    } else if (dir.first_child == -1)
    {
        p = 0;
    } else
    {
        throw std::runtime_error("Bad descriptor in block №" + std::to_string(dir.first_block));
    }
}

dir_iterator &dir_iterator::operator++()
{
    if (p->next_file > 0 && p->next_file < _fs.config.block_no)
    {
        block *_block = new block(p->next_file, _fs.config.block_size, _fs._root);
        _fs.read_data(p, sizeof(file_descriptor), _block);
        return *this;
    } else if (p->next_file == -1)
    {
        p = 0;
        return *this;
    } else
    {
        throw std::runtime_error("Bad descriptor in block №" + std::to_string(p->first_block));
    }
}

void file_system::update_descriptor(file_descriptor &fd, block *block)
{
    if (block->_descriptor.data_written > 0)
    {
        int written = block->_descriptor.data_written;
        block->move_to_begin();
        block->write(&fd, sizeof(fd));
        block->_descriptor.data_written = written;
        block->save_block(_root);
    } else
    {
        block->write(&fd, sizeof(fd));
    }
}
