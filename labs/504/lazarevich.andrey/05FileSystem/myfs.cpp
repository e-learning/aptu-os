#include "myfs.h"

using std::string;
using std::ios;
using std::ifstream;
using std::ofstream;
using std::vector;

MyFS::MyFS()
    :total_size(0)
{
    super_block.block_size = 0;
    super_block.block_nums = 0;
}

MyFS::MyFS(const char *root, bool need_init = true)
    :root_path(root)
{
    if (need_init)
    {
        ifstream config_file ((root_path + string("/config")).c_str());
        if (!config_file.good())
        {
            std::cout << "There is no config file in root directory" << std::endl;
            exit(1);
        }
        config_file >> super_block.block_size;
        if (super_block.block_size < MIN_BLOCK_SIZE)
        {
            std::cout << "Block size is too small" << std::endl;
            exit(1);
        }
        config_file >> super_block.block_nums;
        free_blocks_num = super_block.block_nums;
        super_block.root_id = -1;
    }
    else
    {
        super_block = read_super_block();
        count_tables_sizes();
        blocks_map = read_blocks_map();
        descriptor_table = read_descriptor_table();
        free_blocks_num = count_free_blocks_num();
    }
    total_size = super_block.block_nums * super_block.block_size;
    if (total_size > MAX_DISK_SIZE)
    {
        std::cout << "Disk size is bigger than we expect. Sorry" << std::endl;
        exit(1);
    }

}

void MyFS::count_tables_sizes()
{
    max_possible_files = super_block.block_nums - 2;
    blockn_for_descriptor_table = max_possible_files * sizeof(u_int32_t) /
            (super_block.block_size - sizeof(BlockHeader)) + 1;
}

void MyFS::rewrite_file_info(MyFile file)
{
    if (file.is_dir)
    {
        ofstream out(root_path + get_block_name_by_id(descriptor_table[file.fd]), ios::binary);
        write_data_in_block(out, &file, descriptor_table[file.fd], sizeof(file), 0);
        return;
    }
    size_t buf_size = file.file_size > super_block.block_size ? super_block.block_size - sizeof(file) :
                                                                file.file_size;
    MyFile buf_file;
    char buf[buf_size];
    ifstream in((root_path + get_block_name_by_id(descriptor_table[file.fd])).c_str(), ios::binary);
    in.read((char *)&buf_file, sizeof(buf_file));
    in.read((char *)buf, sizeof(buf));
    in.close();
    ofstream out((root_path + get_block_name_by_id(descriptor_table[file.fd])).c_str(), ios::binary);
    write_data_in_block(out, &file, descriptor_table[file.fd], sizeof(file), 0);
    write_data_in_block(out, &buf, descriptor_table[file.fd], sizeof(buf), sizeof(file));
}

void MyFS::write_new_file(MyFile file, ifstream & in)
{
    size_t size_left = file.file_size;
    ofstream out((root_path + get_block_name_by_id(descriptor_table[file.fd])).c_str(), ios::binary);
    write_data_in_block(out, &file, descriptor_table[file.fd], sizeof(file), 0);
    size_t buf_size = (file.file_size > super_block.block_size - sizeof(file)) ? super_block.block_size - sizeof(file) : file.file_size;
    char buf[buf_size];
    in.read((char *)&buf, sizeof(buf));
    write_data_in_block(out, buf, descriptor_table[file.fd], sizeof(buf), sizeof(file));
    size_t current_block = file.next_block;
    size_left -= sizeof(buf);
    for (size_t i = 1; i < file.num_of_blocks; ++i)
    {
        ofstream out_block((root_path + get_block_name_by_id(current_block).c_str()), ios::binary);
        blocks_map[current_block] = true;
        BlockHeader bh;
        char out_buf[i == file.num_of_blocks - 1 ? size_left : super_block.block_size - sizeof(bh)];
        bh.file_id = file.fd;
        bh.next = (i == file.num_of_blocks - 1) ? -1 : first_free_block();
        bh.occ_space = (i == file.num_of_blocks - 1) ? sizeof(bh) + sizeof(out_buf) : super_block.block_size;
        size_left -= sizeof(out_buf);
        in.read((char *)out_buf, sizeof(out_buf));
        write_data_in_block(out_block, &bh, current_block, sizeof(bh), 0);
        write_data_in_block(out_block, out_buf, current_block, sizeof(out_buf), sizeof(bh));
        current_block = bh.next;
    }
}

void MyFS::write_new_dir(MyFile file)
{
    ofstream out(root_path + get_block_name_by_id(descriptor_table[file.fd]), ios::binary);
    write_data_in_block(out, &file, descriptor_table[file.fd], sizeof(file), 0);
}

void MyFS::delete_file(MyFile &file)
{
    int prev_fd = file.prev_file;
    int next_fd = file.next_file;
    int parent_fd = file.parent;
    MyFile parent;
    MyFile next;
    MyFile prev;
    if (parent_fd != -1)
    {
        parent = read_file_info_by_id(descriptor_table[parent_fd]);
    }
    if (prev_fd == -1 && next_fd == -1)
    {
        parent.first_file_id = -1;
        rewrite_file_info(parent);
    }
    else if (prev_fd != -1 && next_fd != -1)
    {
        next = read_file_info_by_id(descriptor_table[next_fd]);
        prev = read_file_info_by_id(descriptor_table[prev_fd]);
        next.prev_file = prev_fd;
        prev.next_file = next_fd;
        rewrite_file_info(next);
        rewrite_file_info(prev);
    }
    else if (next_fd != -1)
    {
        next = read_file_info_by_id(descriptor_table[next_fd]);
        next.prev_file = -1;
        parent.first_file_id = next_fd;
        rewrite_file_info(parent);
        rewrite_file_info(next);
    }
    else if (prev_fd != -1)
    {
        prev = read_file_info_by_id(descriptor_table[prev_fd]);
        prev.next_file = -1;
        rewrite_file_info(prev);
    }
    file.parent = -1;
    rewrite_file_info(file);
    free_resources(file);
}

void MyFS::free_resources(MyFile &file)
{
    blocks_map[descriptor_table[file.fd]] = false;
    int current_block = file.next_block;
    BlockHeader bh;
    while ( current_block != -1 )
    {
        blocks_map[current_block] = false;
        bh = read_block_by_id(current_block);
        current_block = bh.next;
    }
    if (current_block != -1)
        blocks_map[current_block] = false;
    descriptor_table[file.fd] = -1;
}

bool MyFS::file_exist(const char *name, MyFile dir)
{
    if (get_fd_by_name(dir, name) < 0)
        return false;
    else
         return true;
}

size_t MyFS::count_free_blocks_num()
{
    size_t counter = 0;
    for (size_t i = 0; i < blocks_map.size(); ++i)
    {
        if (!blocks_map[i])
            ++counter;
    }
    return counter;
}

size_t MyFS::first_free_block()
{
    size_t i = 0;
    for (i = 0; i < blocks_map.size(); ++i)
    {
        if (!blocks_map[i])
            break;
    }
    return i;
}

int32_t MyFS::first_free_fd()
{
    unsigned int max_value = -1;
    size_t i = 0;
    for (i = 0; i < descriptor_table.size(); ++i)
    {
        if (descriptor_table[i] == max_value)
        {
            break;
        }
    }
    if (descriptor_table[i] != max_value)
        return -1;
    else
        return i;
}

const char *MyFS::file_preparation(const char *path, MyFile & cur_dir)
{
    if (path[0] != '/')
    {
        return nullptr;
    }
    cur_dir = read_file_info_by_id(super_block.root_id);
    size_t start_pos = 1;
    while (size_t pos = find_next_slash(path, start_pos))
    {
        char cur_name[10];
        strncpy(cur_name, &path[start_pos], pos - start_pos);
        cur_name[pos-start_pos] = '\0';
        int fd = get_fd_by_name(cur_dir, cur_name);
        if (strncmp(read_file_info_by_id(descriptor_table[fd]).name, cur_name, strlen(cur_name)) != 0)
            return nullptr;
        if (fd < 0)
        {
            return nullptr;
        }
        start_pos = pos + 1;
    }
    char *file_name = new char[10];
    strncpy(file_name, &path[start_pos], strlen(path) - start_pos);
    file_name[strlen(path) - start_pos] = '\0';
    return file_name;
}

BlockHeader MyFS::read_block_by_id(size_t id)
{
    ifstream in((root_path + get_block_name_by_id(id)).c_str(), ios::binary);
    BlockHeader bh;
    in.read((char *)&bh, sizeof(bh));
    return bh;
}

MyFile MyFS::read_file_info_by_id(size_t id)
{
    ifstream in((root_path + get_block_name_by_id(id)).c_str(), ios::binary);
    MyFile file;
    in.read((char *)&file, sizeof(file));
    return file;
}

int32_t MyFS::get_fd_by_name(MyFile &file, const char *name)
{
    int32_t fd = -1;
    if ((fd = file.first_file_id) == -1)
    {
        return fd;
    }
    file = read_file_info_by_id(descriptor_table[fd]);
    int next = file.next_file;
    do
    {
        next = file.next_file;
        if (strncmp(name, file.name, strlen(file.name)) == 0)
        {
            break;
        }
        else
        {
            file = read_file_info_by_id(descriptor_table[file.next_file]);
            if (file.next_file == -1 && strncmp(file.name, name, strlen(file.name)))
                return -1;
            fd = file.fd;
        }
    }
    while (next != -1);
    return fd;
}


int32_t MyFS::write_file_into_fs(ifstream & in, MyFile &file, const char *file_name, size_t blocks_n,
                                 size_t file_size, bool is_dir)
{
    size_t current_block = first_free_block();
    int32_t file_fd = first_free_fd();
    blocks_map[current_block] = true;
    descriptor_table[file_fd] = current_block;
    int32_t cur_fd = -1;
    MyFile write_file;
    if ((cur_fd = file.first_file_id) == -1)
    {
        file.first_file_id = file_fd;
        write_file.fd = file_fd;
        write_file.parent = file.fd;
        write_file.is_dir = is_dir;
        write_file.next_file = -1;
        write_file.prev_file = -1;
        write_file.first_file_id = -1;
        write_file.next_block = file_size > super_block.block_size - sizeof(write_file) ? first_free_block() : -1;
        write_file.num_of_blocks = blocks_n;
        write_file.file_size = file_size;
        strncpy(write_file.name, file_name, sizeof(write_file.name));
        rewrite_file_info(file);
        if (!is_dir)
            write_new_file(write_file, in);
        else
            write_new_dir(write_file);
    }
    else
    {
        MyFile cur_file = read_file_info_by_id(descriptor_table[cur_fd]);
        while(cur_file.next_file != -1)
        {
            cur_fd = cur_file.next_file;
            cur_file = read_file_info_by_id(descriptor_table[cur_fd]);
        }
        write_file.next_file = cur_file.next_file;
        cur_file.next_file = file_fd;
        write_file.parent = file.fd;
        write_file.is_dir = is_dir;
        write_file.prev_file = cur_file.fd;
        write_file.first_file_id = -1;
        write_file.num_of_blocks = blocks_n;
        write_file.next_block = file_size > super_block.block_size - sizeof(write_file) ? first_free_block() : -1;
        write_file.file_size = file_size;
        write_file.fd = file_fd;
        strcpy(write_file.name, file_name);
        rewrite_file_info(cur_file);
        if (!is_dir)
            write_new_file(write_file, in);
        else
            write_new_dir(write_file);
    }
    free_blocks_num -= write_file.num_of_blocks;
    write_descriptor_table();
    write_free_blocks_map();
    return file_fd;
}

int32_t MyFS::write_file_from_fs(std::ofstream &out, int32_t fd)
{
    ifstream in(root_path + get_block_name_by_id(descriptor_table[fd]), ios::binary);
    MyFile file;
    in.read((char *)&file, sizeof(file));
    char buf[file.file_size > super_block.block_size - sizeof(file) ? super_block.block_size - sizeof(file) : file.file_size];
    in.read((char *)buf, sizeof(buf));
    out.write((char *)buf, sizeof(buf));
    in.close();
    size_t current_block = file.next_block;
    for (size_t i = 1; i < file.num_of_blocks; ++i)
    {
        in.open(root_path + get_block_name_by_id(current_block), ios::binary);
        BlockHeader bh;
        in.read((char *)&bh, sizeof(bh));
        char out_buf[bh.occ_space - sizeof(bh)];
        in.read((char *)out_buf, sizeof(out_buf));
        out.write((char *)out_buf, sizeof(out_buf));
        current_block = bh.next;
        in.close();
    }
    return 0;
}

size_t MyFS::memory_to_write(int32_t file_size)
{
    size_t block_counter = 0;
    file_size += sizeof(MyFile);
    file_size -= super_block.block_size;
    ++block_counter;
    while (file_size > 0)
    {
        file_size += sizeof(BlockHeader);
        file_size -= super_block.block_size;
        ++block_counter;
    }
    return block_counter;
}


int MyFS::init()
{
    create_blocks_in_root();
    write_super_block();
    count_tables_sizes();
    blocks_map.resize(super_block.block_nums);
    std::fill(blocks_map.begin(), blocks_map.begin() + 2 + blockn_for_descriptor_table, true);
    std::fill(blocks_map.begin() + 3 + blockn_for_descriptor_table, blocks_map.end(), false);
    write_free_blocks_map();
    free_blocks_num -= 2;
    descriptor_table.resize(max_possible_files);
    std::fill(descriptor_table.begin(), descriptor_table.end(), -1);
    write_descriptor_table();
    free_blocks_num -= blockn_for_descriptor_table;
    return 0;
}

int MyFS::format()
{
    MyFile root_dir;
    root_dir.first_file_id = -1;
    root_dir.fd = 0;
    root_dir.is_dir = true;
    strcpy(root_dir.name, "/");
    root_dir.next_file = -1;
    root_dir.prev_file = -1;
    root_dir.num_of_blocks = 1;
    root_dir.parent = -1;
    root_dir.file_size = 0;
    descriptor_table[0] = 2 + blockn_for_descriptor_table;
    blocks_map[descriptor_table[0]] = true;
    write_free_blocks_map();
    free_blocks_num -= root_dir.num_of_blocks;
    write_descriptor_table();

    ofstream out((root_path + get_block_name_by_id(descriptor_table[0])).c_str(), ios::binary);
    write_data_in_block(out, &root_dir, descriptor_table[0], sizeof(root_dir), 0);
    super_block.root_id = descriptor_table[0];
    write_super_block();
    return 0;
}

int MyFS::import_file(const char *from, const char *to)
{
    ifstream source(from, ios::ate | ios::binary);
    size_t file_size = source.tellg();
    size_t blocks_req = memory_to_write(file_size);
    if (blocks_req > free_blocks_num)
    {
        return -1;
    }
    else
    {
        source.close();
        source.open(from, ios::binary);
    }
    MyFile cur_dir;
    const char *file_name = file_preparation(to, cur_dir);
    if (file_name == nullptr)
        return -1;
    int fd = write_file_into_fs(source, cur_dir, file_name, blocks_req, file_size, false);
    return fd;
}

int MyFS::export_file(const char *from, const char *to)
{
    MyFile cur_dir;
    const char *file_name = file_preparation(from, cur_dir);
    if (file_name == nullptr)
        return -1;
    int fd = get_fd_by_name(cur_dir, file_name);
    if (fd < 0)
    {
        return -1;
    }
    ofstream out(to, ios::binary);
    return write_file_from_fs(out, fd);
}

int MyFS::make_dir(MyFile cur_dir, const char *dir_name)
{
    MyFile dir;
    dir.fd = first_free_fd();
    if (dir.fd < 0)
        return -1;
    descriptor_table[dir.fd] = first_free_block();
    blocks_map[descriptor_table[dir.fd]] = true;
    dir.next_block = -1;
    dir.next_file = -1;
    dir.first_file_id = -1;
    dir.is_dir = true;
    dir.num_of_blocks = 1;
    dir.parent = cur_dir.fd;
    dir.file_size = 0;
    strncpy(dir.name, dir_name, strlen(dir_name));
    dir.name[strlen(dir_name)] = '\0';
    dir.prev_file = -1;
    if (cur_dir.first_file_id != -1)
    {
        int cur_fd = cur_dir.first_file_id;
        MyFile first_file = read_file_info_by_id(descriptor_table[cur_fd]);
        first_file.prev_file = dir.fd;
        dir.next_file = first_file.fd;
        rewrite_file_info(first_file);
    }
    cur_dir.first_file_id = dir.fd;
    rewrite_file_info(cur_dir);
    ofstream out(root_path + get_block_name_by_id(descriptor_table[dir.fd]), ios::binary);
    write_data_in_block(out, &dir, descriptor_table[dir.fd], sizeof(dir), 0);
    return dir.fd;
}


int MyFS::mkdir(const char *path)
{
    MyFile cur_dir = read_file_info_by_id(super_block.root_id);
    size_t start_pos = 1;
    int fd = 0;
    int cur_pos = 0;
    while ((cur_pos = find_next_slash(path, start_pos)) != 0)
    {
        char dir_name[10];
        strncpy(dir_name, &path[start_pos], cur_pos - start_pos);
        dir_name[cur_pos - start_pos] = '\0';
        start_pos = cur_pos + 1;
        if (file_exist(dir_name, cur_dir))
        {
            MyFile buf = cur_dir;
            int cur_fd = get_fd_by_name(buf, dir_name);
            cur_dir = read_file_info_by_id(descriptor_table[cur_fd]);
        }
        else
        {
            fd = make_dir(cur_dir, dir_name);
            if (fd < 0)
                return -1;
            cur_dir = read_file_info_by_id(descriptor_table[fd]);
        }
    }
    char dir_name[10];
    strncpy(dir_name, &path[start_pos], strlen(path) - start_pos);
    dir_name[strlen(path) - start_pos] = '\0';
    if (file_exist(dir_name, cur_dir))
    {
        return -1;
    }
    else
    {
        fd = make_dir(cur_dir, dir_name);
        if (fd < 0)
            return -1;
    }
    write_descriptor_table();
    write_free_blocks_map();
    return fd;
}

std::string MyFS::ls(const char *path)
{
    string result;
    MyFile cur_dir;
    const char *file_name = file_preparation(path, cur_dir);
    int fd = 0;
    if (strncmp(path, "/", strlen(path)))
    {
        if (file_name == nullptr)
            return 0;
        fd = get_fd_by_name(cur_dir, file_name);
        if (fd < 0)
        {
            result += string("Directory or file doesn't exist");
            return result;
        }
    }
    MyFile file = read_file_info_by_id(descriptor_table[fd]);
    if (file.is_dir)
    {
        int cur_fd = file.first_file_id;
        if (cur_fd == -1)
        {
            result += string("Your directory is empty");
        }
        else
        {
            MyFile cur_file;
            while ((cur_file = read_file_info_by_id(descriptor_table[cur_fd])).next_file != -1)
            {
                if (cur_file.parent == cur_dir.fd)
                {
                    result += string(cur_file.name);
                    result += cur_file.is_dir ? string(" d\n") : string(" f\n");
                    cur_fd = cur_file.next_file;
                }
            }
            if (cur_file.parent == cur_dir.fd)
            {
                cur_file = read_file_info_by_id(descriptor_table[cur_fd]);
                result += string(cur_file.name);
                result += cur_file.is_dir ? string(" d") : string(" f");
            }
        }
    }
    else
    {
        result += string(path) + " " + std::to_string(file.num_of_blocks);
    }
    return result;
}

int MyFS::move(const char *from, const char *to)
{
    MyFile cur_dir;
    const char *file_name = file_preparation(from, cur_dir);
    MyFile old_parent = cur_dir;
    int fd = get_fd_by_name(cur_dir, file_name);
    MyFile file = read_file_info_by_id(descriptor_table[fd]);
    int prev_fd = file.prev_file;
    int next_fd = file.next_file;
    if (next_fd == -1 && prev_fd == -1)
    {
        old_parent.first_file_id = -1;
        rewrite_file_info(old_parent);
    }
    else if (next_fd != -1 && prev_fd != -1)
    {
        MyFile prev_file = read_file_info_by_id(descriptor_table[prev_fd]);
        MyFile next_file = read_file_info_by_id(descriptor_table[next_fd]);
        prev_file.next_file = file.next_file;
        next_file.prev_file = file.prev_file;
        rewrite_file_info(prev_file);
        rewrite_file_info(next_file);
    }
    else if (next_fd == -1)
    {
        MyFile prev_file = read_file_info_by_id(descriptor_table[prev_fd]);
        prev_file.next_file = -1;
        rewrite_file_info(prev_file);
    }
    else if (prev_fd == -1)
    {
        MyFile next_file = read_file_info_by_id(descriptor_table[next_fd]);
        next_file.prev_file = -1;
        old_parent.first_file_id = next_file.fd;
        rewrite_file_info(old_parent);
        rewrite_file_info(next_file);
    }
    MyFile new_parent_dir;
    const char *new_name = file_preparation(to, new_parent_dir);
    if (new_parent_dir.first_file_id == -1)
    {
        file.next_file = -1;
    }
    else
    {
        MyFile first_file = read_file_info_by_id(descriptor_table[new_parent_dir.first_file_id]);
        first_file.prev_file = file.fd;
        file.next_file = first_file.fd;
        rewrite_file_info(first_file);
    }
    new_parent_dir.first_file_id = file.fd;
    strncpy(file.name, new_name, strlen(new_name));
    file.parent = new_parent_dir.fd;
    file.prev_file = -1;
    rewrite_file_info(file);
    rewrite_file_info(new_parent_dir);
    return 0;
}

int MyFS::copy(const char *from, const char *to)
{
    MyFile src_file;
    MyFile dst_file;
    const char *src_name = file_preparation(from, src_file);
    int src_fd = get_fd_by_name(src_file, src_name);
    if (src_fd < 0)
    {
        std::cout << "File doesn't exist" << std::endl;
        return -1;
    }
    const char *dst_name = file_preparation(to, dst_file);
    if ( copy(src_file, dst_file, dst_name) < 0)
    {
        std::cout << "Copy error" << std::endl;
        return -1;
    }
    write_descriptor_table();
    write_free_blocks_map();
    return 0;
}


void MyFS::rm(MyFile &file)
{
    if (file.is_dir)
    {
        if (file.first_file_id != -1)
        {
            int cur_fd = file.first_file_id;
            do
            {
                MyFile cur_file = read_file_info_by_id(descriptor_table[cur_fd]);
                cur_fd = cur_file.next_file;
                rm(cur_file);
            }
            while (cur_fd != -1);
        }
        delete_file(file);
    }
    else
    {
        delete_file(file);
    }
}

int MyFS::copy(MyFile &src, MyFile &dst, const char *new_name)
{
    if (src.is_dir)
    {
        MyFile copy_dir = src;
        copy_dir.first_file_id = -1;
        int dir_fd = copy_file_to_dir(copy_dir, dst, new_name);
        if (dir_fd < 0)
        {
            return -1;
        }
        MyFile cur_dir = read_file_info_by_id(descriptor_table[dir_fd]);
        if (src.first_file_id != -1)
        {
            int cur_fd = src.first_file_id;
            do
            {
                MyFile cur_file = read_file_info_by_id(descriptor_table[cur_fd]);
                cur_fd = cur_file.next_file;
                copy(cur_file, cur_dir, cur_file.name);
            }
            while (cur_fd != -1);
        }
    }
    else
    {
        if (copy_file_to_dir(src, dst, new_name) < 0)
            return -1;
    }
    return 0;
}

int MyFS::copy_file_to_dir(MyFile &file, MyFile &dir, const char *new_name)
{
    MyFile copy_file = file;
    if (copy_file.num_of_blocks > count_free_blocks_num())
    {
        return -1;
    }
    copy_file.prev_file = -1;
    copy_file.next_file = -1;
    copy_file.parent = dir.fd;
    copy_file.fd = first_free_fd();
    strncpy(copy_file.name, new_name, strlen(new_name));
    int32_t current_block = first_free_block();
    descriptor_table[copy_file.fd] = current_block;
    blocks_map[current_block] = true;
    if (dir.first_file_id != -1)
    {
        MyFile first_file = read_file_info_by_id(descriptor_table[dir.first_file_id]);
        first_file.prev_file = copy_file.fd;
        copy_file.next_file = first_file.fd;
        rewrite_file_info(first_file);
    }
    dir.first_file_id = copy_file.fd;
    rewrite_file_info(dir);
    copy_file.next_block = copy_file.file_size > super_block.block_size - sizeof(copy_file) ? first_free_block() : -1;
    ofstream out(root_path + get_block_name_by_id(descriptor_table[copy_file.fd]), ios::binary);
    ifstream in(root_path + get_block_name_by_id(descriptor_table[file.fd]), ios::binary);
    in.seekg(sizeof(file));
    write_data_in_block(out, &copy_file, descriptor_table[copy_file.fd], sizeof(copy_file), 0);
    size_t buf_size = (file.file_size > super_block.block_size - sizeof(file)) ? super_block.block_size - sizeof(file) : file.file_size;
    char buffer[buf_size];
    in.read((char *)buffer, sizeof(buffer));
    write_data_in_block(out, buffer, descriptor_table[copy_file.fd], sizeof(buffer), sizeof(copy_file));
    if (copy_file.next_block != -1)
    {
        size_t block_to_copy = file.next_block;
        current_block = copy_file.next_block;
        do
        {
            blocks_map[current_block] = true;
            out.close();
            in.close();
            out.open(root_path + get_block_name_by_id(current_block), ios::binary);
            in.open(root_path + get_block_name_by_id(block_to_copy), ios::binary);

            BlockHeader orig_bh;
            BlockHeader copy_bh;
            in.read((char *)&orig_bh, sizeof(orig_bh));
            copy_bh = orig_bh;
            copy_bh.next = orig_bh.next == -1 ? -1 : first_free_block();
            write_data_in_block(out, &copy_bh, current_block, sizeof(copy_bh), 0);
            buf_size = copy_bh.occ_space - sizeof(copy_bh);
            char out_buf[buf_size];
            in.read((char *)out_buf, sizeof(out_buf));
            write_data_in_block(out, out_buf, current_block, sizeof(out_buf), sizeof(copy_bh));

            block_to_copy = orig_bh.next;
            current_block = copy_bh.next;
        }
        while (current_block != -1);
    }
    return copy_file.fd;
}


int MyFS::rm(const char *path)
{
    MyFile cur_dir;
    const char *name = file_preparation(path, cur_dir);
    int fd = get_fd_by_name(cur_dir, name);
    if (fd < 0)
        return -1;
    MyFile file = read_file_info_by_id(descriptor_table[fd]);
    rm(file);
    write_descriptor_table();
    write_free_blocks_map();
    return 0;
}

vector<bool> MyFS::read_blocks_map()
{
    ifstream in((root_path + get_block_name_by_id(1)).c_str(), ios::binary);
    vector<bool>return_vector(super_block.block_nums);
    BlockHeader bh;
    in.read((char *)&bh, sizeof(bh));
    char buffer = 0;
    int bit_counter = 0;
    size_t i = 0;
    while( i < return_vector.size() )
    {
        if (bit_counter == 0)
            in.get(buffer);
        bool bit = buffer & (1 << (7 - bit_counter));
        return_vector[i] = bit;
        ++i;
        ++bit_counter;
        if (bit_counter == 8)
        {
            bit_counter = 0;
        }
    }
    return return_vector;
}


vector<u_int32_t> MyFS::read_descriptor_table()
{
    vector<u_int32_t> return_vector(max_possible_files);
    ifstream in((root_path + get_block_name_by_id(2)).c_str(), ios::binary);
    BlockHeader bh;
    in.read((char *)&bh, sizeof(bh));
    size_t current_index = 0;
    while (!in.eof() && current_index < return_vector.size())
    {
        u_int32_t buf;
        in.read((char *)&buf, sizeof(buf));
        return_vector[current_index] = buf;
        ++current_index;
    }
    while (bh.next != -1)
    {
        current_index--; //one down, cause in.eof()
        in.close();
        in.open((root_path + get_block_name_by_id(bh.next)).c_str(), ios::binary);
        in.read((char *)&bh, sizeof(bh));
        while (!in.eof() && current_index < return_vector.size())
        {
            u_int32_t buf;
            in.read((char *)&buf, sizeof(buf));
            return_vector[current_index] = buf;
            ++current_index;
        }
    }
    return return_vector;
}


SuperBlock MyFS::read_super_block()
{
    ifstream first_block((root_path + get_block_name_by_id(0)).c_str(), ios::binary);
    SuperBlock sb;
    first_block.read((char *)&sb, sizeof(sb));
    return sb;
}

void MyFS::write_super_block()
{
    ofstream out(root_path + get_block_name_by_id(0), ios::binary);
    SuperBlock sb;
    sb = super_block;
    write_data_in_block(out, &sb, 0, sizeof(sb), 0);
}

void MyFS::write_free_blocks_map()
{
    ofstream out(root_path + get_block_name_by_id(1), ios::binary);
    BlockHeader bh;
    bh.file_id = -1;
    bh.next = -1;
    bh.occ_space = blocks_map.size() + sizeof(bh);
    write_data_in_block(out, &bh, 1, sizeof(bh), 0);
    out.seekp(sizeof(bh));
    char buffer = 0;
    size_t bit_counter = 0;
    for (size_t i = 0; i < blocks_map.size(); ++i)
    {
        buffer = buffer | blocks_map[i] << ( 7 - bit_counter);
        ++bit_counter;
        if (i == blocks_map.size() - 1)
        {
            out.write((char *)&buffer, sizeof(buffer));
        }
        else if (bit_counter == 8)
        {
            out.write((char *)&buffer, sizeof(buffer));
            bit_counter = 0;
            buffer = 0;
        }
    }
    buffer = '\0';
    out.write((char *)&buffer, super_block.block_size - bh.occ_space);
}

void MyFS::write_descriptor_table()
{
    size_t current_index = 0;
    if (!descriptor_table.empty())
    {
        for (size_t i = 1; i <= blockn_for_descriptor_table; ++i)
        {
            ofstream out(root_path + get_block_name_by_id(i + 1), ios::binary);
            if (i == blockn_for_descriptor_table)
            {
                BlockHeader bh;
                bh.file_id = -1;
                bh.next = -1;
                bh.occ_space = sizeof(bh) + (descriptor_table.size() - current_index) * sizeof(u_int32_t);
                write_data_in_block(out, &bh, i + 1, sizeof(bh), 0);
                out.seekp(sizeof(bh));
                while (current_index < descriptor_table.size())
                {
                    out.write((char *)&descriptor_table[current_index], sizeof(u_int32_t));
                    ++current_index;
                }
                char buffer = '\0';
                out.write((char *)&buffer, super_block.block_size - bh.occ_space);
                out.close();
            }
            else
            {
                BlockHeader bh;
                bh.file_id = -1;
                bh.next = i + 2;
                bh.occ_space = super_block.block_size;
                write_data_in_block(out, &bh, i + 1, sizeof(bh), 0);
                out.seekp(sizeof(bh));
                while ( current_index * sizeof(u_int32_t) < i * (super_block.block_size - sizeof(bh)))
                {
                    out.write((char *)&descriptor_table[current_index], sizeof(u_int32_t));
                    ++current_index;
                }
            }\
        }
    }
}

void MyFS::create_blocks_in_root()
{
    for (size_t i = 0; i < super_block.block_nums; ++i)
    {
        FILE *fp = fopen((root_path + get_block_name_by_id(i)).c_str(), "w");
        fseek(fp, super_block.block_size - 1, SEEK_SET);
        fputc('\0', fp);
        fclose(fp);
    }
}

ofstream & MyFS::write_data_in_block(ofstream & out, void * data, int id, size_t length, size_t pos)
{
    if (!out)
    {
        out.open(root_path + get_block_name_by_id(id), std::ios::binary);
    }
    out.seekp(pos);
    out.write((char *)data, length);
    if (out.tellp() < (int)super_block.block_size)
    {
        char c = '\0';
        out.write((char *)&c, super_block.block_size - length);
    }
    return out;
}

string MyFS::get_block_name_by_id(int id)
{
    return (string("/block") + std::to_string(id));
}


size_t find_next_slash(const char *path, size_t cur_pos)
{
    ++cur_pos;
    while (cur_pos < strlen(path))
    {
        if (path[cur_pos] == '/')
            break;
        ++cur_pos;
    }
    if (cur_pos >= strlen(path))
    {
        return 0;
    }
    return cur_pos;
}
