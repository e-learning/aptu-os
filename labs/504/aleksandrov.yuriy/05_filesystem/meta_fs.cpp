#include <math.h>
#include <cstring>
#include <string>
#include <sstream>
#include "meta_fs.h"

using namespace std;


Meta_FS::Meta_FS(const char *root)
    : root(root)
    , config(get_config())
    , block_size(config.block_size - sizeof(Block))
    , is_init(ifstream(get_block_path(0), ios::binary))
{
    if (is_init)
    {
        root_dir = get_inode_by_id(0);
        read_block_map();
    }
}

Meta_FS::~Meta_FS()
{
    if (is_init)
    {
        write_block_map();
    }
}

Config Meta_FS::get_config()
{
    Config config;
    ifstream conf_file((string(root) + "/config").c_str());
    if (!conf_file.good())
    {
        throw FSException("Can't open config file");
    }
    conf_file >> config.block_size >> config.blocks_num;
    return config;
}

string Meta_FS::get_block_path(const int32_t id) const
{
    return string(root) + "/block" + to_string(id);
}

void Meta_FS::format()
{
    if (!is_init)
    {
        throw FSException("Filesystem hasn't initialised!");
    }
    root_dir = init_inode("/");
    root_dir.id = 0;
    is_free_block.assign(config.blocks_num, false);
    int32_t free_blocks = static_cast<int32_t>(ceil((config.blocks_num / 8.0 + sizeof(Inode)) / config.block_size));
    for (int32_t i = 0; i < free_blocks; ++i)
    {
        is_free_block[i] = true;
    }
}

void Meta_FS::rm(const Inode &inode)
{
    if (inode.is_dir == false)
    {
        clear_inode(inode);
    }
    else
    {
        if (inode.first_child != -1)
        {
            int32_t next_child = inode.first_child;
            do
            {
                Inode child = get_inode_by_id(next_child);
                rm(child);
                next_child = child.next_inode;
            }
            while (next_child != -1);
        }
    }
    remove_inode(inode.id);
}

vector<Inode> Meta_FS::ls(const Inode & directory) const
{
    vector<Inode> inodes;
    if (directory.first_child != -1)
    {
        inodes.push_back(get_inode_by_id(directory.first_child));
        while (inodes.back().next_inode != -1)
        {
            inodes.push_back(get_inode_by_id(inodes.back().next_inode));
        }
    }
    return inodes;
}

Inode Meta_FS::init_inode(const string &name, bool is_dir)
{
    Inode inode;
    inode.id = -1;
    inode.is_dir = is_dir;
    strncpy(inode.name, name.c_str(), sizeof(inode.name));
    inode.first_block = -1;
    inode.blocks_num = 0;
    inode.first_child = -1;
    inode.parent_inode = -1;
    inode.next_inode = -1;
    inode.prev_inode = -1;
    return inode;
}

void Meta_FS::clear_inode(const Inode &inode) {
    if (inode.first_block != -1)
    {
        Block block = read_block(inode.first_block);
        while (block.next != -1)
        {
            is_free_block[block.id] = false;
            block = read_block(block.next);
        }
        is_free_block[block.id] = false;
    }
}

void Meta_FS::remove_inode(const int32_t id)
{
    Inode inode = get_inode_by_id(id);
    if (inode.prev_inode == -1)
    {
        if (inode.next_inode == -1)
        {
            Inode parent = get_inode_by_id(inode.parent_inode);
            parent.first_child = -1;
            write_inode(parent);
        }
        else
        {
            Inode parent = get_inode_by_id(inode.parent_inode);
            Inode next = get_inode_by_id(inode.next_inode);
            parent.first_child = next.id;
            next.parent_inode = parent.id;
            next.prev_inode = -1;
            write_inode(parent);
            write_inode(next);
        }
    }
    else
    {
        if (inode.next_inode == -1)
        {
            Inode prev = get_inode_by_id(inode.prev_inode);
            prev.next_inode = -1;
            write_inode(prev);
        }
        else
        {
            Inode prev = get_inode_by_id(inode.prev_inode);
            Inode next = get_inode_by_id(inode.next_inode);
            prev.next_inode = next.id;
            next.prev_inode = prev.id;
            write_inode(prev);
            write_inode(next);
        }
    }
    is_free_block[inode.id] = false;
}

Inode Meta_FS::get_inode(Inode &directory, const string &inode_name, bool create, bool is_directory)
{
    if (directory.first_child == -1)
    {
        if (create)
        {
            uint32_t new_block_id = get_available_block();
            Inode new_inode = init_inode(inode_name, is_directory);
            directory.first_child = new_inode.id = new_block_id;
            new_inode.parent_inode = directory.id;
            write_inode(directory);
            write_inode(new_inode);
            return new_inode;
        }
    }
    else
    {
        Inode current = get_inode_by_id(directory.first_child);
        while (inode_name != current.name && current.next_inode != -1)
        {
            current = get_inode_by_id(current.next_inode);
        }
        if (inode_name == current.name)
        {
            return current;
        }
        else
        {
            if (create)
            {
                uint32_t new_block_id = get_available_block();
                Inode new_file = init_inode(inode_name, is_directory);
                current.next_inode = new_file.id = new_block_id;
                new_file.prev_inode = current.id;
                write_inode(current);
                write_inode(new_file);
                return new_file;
            }
        }
    }
    throw FSException("Can't find '" + inode_name + "'");
}

vector<string> split_string(const string &str, const char delimiter)
{
    stringstream test(str);
    string segment;
    vector<string> seglist;
    while(getline(test, segment, delimiter))
    {
        seglist.push_back(segment);
    }
    return seglist;
}

Inode Meta_FS::get_inode(const char *dst, bool create /* = true */, bool is_dir /* = false */)
{
    vector<string> path = split_string(string(dst), '/');
    string inode_name = path.back();
    path.pop_back();
    if (inode_name == "")
    {
        return root_dir;
    }
    Inode directory = get_directory(path, create);
    return get_inode(directory, inode_name, create, is_dir);
}

Inode Meta_FS::get_directory(const char *dst, bool create)
{
    return get_directory(split_string(dst, '/'), create);
}

Inode Meta_FS::get_directory(const vector<string> &path, bool create)
{
    if (path.size() < 1 || path[0] != "")
    {
        throw FSException("Invalid path");
    }
    Inode curr_inode = root_dir;
    for (vector<string>::const_iterator iter = path.begin() + 1; iter != path.end(); ++iter)
    {
        if (curr_inode.first_child == -1)
        {
            if (create)
            {
                Inode new_directory = init_inode(*iter);
                curr_inode.first_child = new_directory.id = get_available_block();
                new_directory.parent_inode = curr_inode.id;
                write_inode(curr_inode);
                write_inode(new_directory);
                curr_inode = new_directory;
            }
            else
            {
                throw FSException("Can't find '" + *iter + "'");
            }
        }
        else
        {
            curr_inode = get_inode_by_id(curr_inode.first_child);
            while (*iter != curr_inode.name && curr_inode.next_inode != root_dir.id && curr_inode.next_inode != -1)
            {
                curr_inode = get_inode_by_id(curr_inode.next_inode);
            }
            if (*iter == curr_inode.name)
            {
                if (!curr_inode.is_dir)
                {
                    throw FSException("'" + *iter + "' isn't a directory");
                }
            }
            else
            {
                if (create)
                {
                    Inode new_directory = init_inode(*iter);
                    curr_inode.next_inode = new_directory.id = get_available_block();
                    new_directory.prev_inode = curr_inode.id;
                    write_inode(curr_inode);
                    write_inode(new_directory);
                    curr_inode = new_directory;
                }
                else
                {
                    throw FSException("Can't find '" + *iter + "'");
                }
            }
        }
    }
    return curr_inode;
}

void Meta_FS::read_data(Inode const &inode, ostream &dst_stream)
{
    char * buffer = new char[block_size];
    int32_t next = inode.first_block;
    while (next != -1)
    {
        ifstream block_stream(get_block_path(next), ios::binary);
        Block block;
        block_stream.read((char *) &block, sizeof(Block));
        block_stream.read(buffer, block.length);
        block_stream.close();
        if (!block_stream)
        {
            delete [] buffer;
            throw FSException("Can't read block");
        }
        dst_stream.write(buffer, block.length);
        next = block.next;
    }
    delete [] buffer;
}

void Meta_FS::write_data(Inode &inode, istream &src_stream)
{
    clear_inode(inode);
    char * buf = new char[block_size];
    int32_t id = inode.first_block = get_available_block();
    while (src_stream)
    {
        src_stream.read(buf, block_size);
        Block block;
        block.id = id;
        block.length = src_stream.gcount();
        block.next = src_stream ? get_available_block(id) : -1;
        is_free_block[block.id] = true;
        ofstream block_stream(get_block_path(id), ios::binary | ios::trunc);
        block_stream.write((char *) &block, sizeof(Block));
        block_stream.write(buf, block.length);
        block_stream.close();
        if (!block_stream)
        {
            throw FSException("Can't write block");
        }
        ++inode.blocks_num;
        id = block.next;
    }
    write_inode(inode);
}

Inode Meta_FS::get_inode_by_id(int32_t id) const
{
    Inode inode;
    ifstream block_stream(get_block_path(id), ios::binary);
    block_stream.read((char *) &inode, sizeof(Inode));
    if (block_stream)
    {
        return inode;
    }
    else
    {
        throw FSException("Can't read inode");
    }
}
Block Meta_FS::read_block(int32_t id) const
{
    Block block;
    ifstream block_stream(get_block_path(id), ios::binary);
    block_stream.read((char *) &block, sizeof(Block));
    if (block_stream)
    {
        return block;
    }
    else
    {
        throw FSException("Can't read inode");
    }
}

void Meta_FS::write_inode(const Inode &inode)
{
    if (inode.id == 0)
    {
        root_dir = inode;
    }
    ofstream block_stream(get_block_path(inode.id), ios::binary);
    block_stream.write((char *) &inode, sizeof(Inode));
    if (!block_stream)
    {
        throw FSException("Can't write file");
    }
    is_free_block[inode.id] = true;
}

void Meta_FS::read_block_map()
{
    is_free_block.clear();
    is_free_block.assign(config.blocks_num, false);
    int curr_block_id = 0;
    ifstream block(get_block_path(curr_block_id), ios::in|ios::binary);
    block.seekg(sizeof(Inode));
    for (int32_t i = 0; i < config.blocks_num; )
    {
        char byte;
        block >> byte;
        for (int32_t b = 0; b < 8 && i < config.blocks_num; ++b, ++i)
        {
            is_free_block[i] = byte & (1 << b);
        }
        if (block.tellg() == config.block_size){
            block.close();
            block.open(get_block_path(++curr_block_id), ios::binary);
        }
    }
}

void Meta_FS::write_block_map()
{
    int32_t curr_block_id = 0;
    ofstream block_stream(get_block_path(curr_block_id), ios::binary|ios::trunc);
    block_stream.write((char *)&root_dir, sizeof(Inode));
    if (!block_stream)
    {
        throw FSException("Can't open block");
    }
    block_stream.seekp(sizeof(Inode));
    for (int32_t i = 0; i < config.blocks_num; )
    {
        char byte = 0;
        for (int b = 0; b < 8 && i < config.blocks_num; ++b, ++i)
        {
            byte |= is_free_block[i] << b;
        }
        block_stream << byte;
        if (!block_stream)
        {
            throw FSException("Write error");
        }
        if (block_stream.tellp() == config.block_size)
        {
            block_stream.close();
            block_stream.open(get_block_path(++curr_block_id), ios::binary);
            if (!block_stream)
            {
                throw FSException("Can't open block");
            }
        }
    }
    block_stream.close();
}


void Meta_FS::check_availability(ifstream & in) const
{
    in.seekg(0, in.end);
    double stream_size = static_cast<double>(in.tellg());
    size_t req_block_num = static_cast<size_t>(ceil(stream_size / config.block_size));
    size_t avail_blocks_num = 0;
    for (size_t i = 1; i < is_free_block.size(); ++i)
    {
        if (!is_free_block[i])
        {
            ++avail_blocks_num;
        }
    }
    in.seekg(0, in.beg);
    
    if (avail_blocks_num < req_block_num)
    {
        throw FSException("Too little available space");
    }
    
    return;
}

void Meta_FS::check_availability(size_t req_blocks_num) const
{
    size_t avail_blocks_num = 0;
    for (size_t i = 1; i < is_free_block.size(); ++i)
    {
        if (!is_free_block[i])
        {
            ++avail_blocks_num;
        }
    }
    if (avail_blocks_num < req_blocks_num)
    {
        throw FSException("Too little available space");
    }
    
    return;
}

size_t Meta_FS::get_available_block(const int32_t start) const
{
    for (size_t i = start + 1; i < is_free_block.size(); ++i)
    {
        if (!is_free_block[i])
        {
            return i;
        }
    }
    throw FSException("Too little available space");
}

void Meta_FS::cp(const Inode &from, Inode &to) {
    if (from.is_dir && !to.is_dir)
    {
        throw FSException(string("'") + to.name + "' is a file");
    }
    else  if (!from.is_dir && to.is_dir)
    {
        Inode new_file = get_inode(to, from.name, true, false);
        cp(from, new_file);
    }
    else if (from.is_dir && to.is_dir)
    {
        if (from.first_child != -1)
        {
            int32_t next_child = from.first_child;
            while (next_child != -1)
            {
                Inode child = get_inode_by_id(next_child);
                Inode new_inode = get_inode(to, child.name, true, child.is_dir);
                cp(child, new_inode);
                next_child = child.next_inode;
            }
        }
    }
    else if (!from.is_dir && !to.is_dir)
    {
        clear_inode(to);
        char * buf = new char[block_size];
        int32_t next_src = from.first_block;
        int32_t next_dst = to.first_block = get_available_block();
        while (next_src != -1)
        {
            Block block;
            ifstream block_stream(get_block_path(next_src), ios::binary);
            block_stream.read((char *) &block, sizeof(Block));
            block_stream.read(buf, block.length);
            block_stream.close();
            next_src = block.next;
            Block new_block = block;
            new_block.id = next_dst;
            new_block.next = next_dst = (next_src == -1 ? -1 : get_available_block(new_block.id));
            ofstream new_block_stream(get_block_path(new_block.id), ios::binary);
            new_block_stream.write((char *)&new_block, sizeof(Block));
            new_block_stream.write(buf, new_block.length);
            is_free_block[new_block.id] = true;
        }
        to.blocks_num = from.blocks_num;
        write_inode(to);
    }
}

void Meta_FS::mv(const Inode &src_inode, Inode &dst_inode) {
    if (src_inode.is_dir)
    {
        if (dst_inode.is_dir)
        {
            if (src_inode.first_child != 1)
            {
                Inode first_child = get_inode_by_id(src_inode.first_child);
                if (dst_inode.first_child == -1)
                {
                    dst_inode.first_child = first_child.id;
                    first_child.parent_inode = dst_inode.id;
                    write_inode(dst_inode);
                }
                else
                {
                    Inode last_child = get_inode_by_id(dst_inode.first_child);
                    while (last_child.next_inode != -1)
                    {
                        last_child = get_inode_by_id(last_child.next_inode);
                    }
                    last_child.next_inode = first_child.id;
                    first_child.prev_inode = last_child.id;
                    write_inode(last_child);
                }
                write_inode(first_child);
            }
            remove_inode(src_inode.id);
        }
        else
        {
            throw FSException("Can't move directory to file");
        }
    }
    else
    {
        if (dst_inode.is_dir)
        {
            dst_inode = get_inode(dst_inode, src_inode.name);
        }
        clear_inode(dst_inode);
        dst_inode.blocks_num = src_inode.blocks_num;
        dst_inode.first_block = src_inode.first_block;
        write_inode(dst_inode);
        remove_inode(src_inode.id);
    }
}
