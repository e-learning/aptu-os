#include <stdint.h>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <sstream>
#include <string.h>
#include <iostream>

#include "fd.cpp"
#include "bd.cpp"

class FS
{
private:
    const std::string root_path_;

    FD* root_;
    uint32_t block_size_;
    uint32_t blocks_;
    std::vector<bool> blocks_using_;

    bool init_;
    uint32_t service_blocks_;
    uint32_t block_data_size_;

    enum fd_type { FDT_DIR, FDT_FILE, FDT_ANY };

public:
    FS(const std::string root_path) : root_path_(root_path)
    {
        root_ = nullptr;

        read_config(root_path_, block_size_, blocks_);
        log(std::string("Block size: ") + std::to_string(block_size_));
        log(std::string("Blocks: ") + std::to_string(blocks_));

        init_ = zero_block_exists(root_path_);
        if (init_)
        {
            log("Zero block exists");
        }
        else
        {
            log("Zero block doesnt exist");
        }

        block_data_size_ = block_size_ - sizeof(BD);
        log(std::string("Block data size: ") + std::to_string(block_data_size_));

        if (init_)
        {
            service_blocks_ = calculate_service_blocks(block_size_, blocks_);
            log(std::string("Service blocks: ") + std::to_string(service_blocks_));

            root_ = read_file_descriptor(root_path_, 0);
            blocks_using_ = read_blocks_using(root_path, blocks_ - service_blocks_, block_size_);
        }
    }

    ~FS()
    {
        if (init_)
        {
            write_main(root_path_, root_, blocks_using_, block_size_);
        }

        if (root_ != nullptr)
        {
            delete root_;
        }
    }

    int init()
    {
        for (uint32_t block = 0; block < blocks_; ++block)
        {
            const std::string block_path = get_block_path(root_path_, block);

            std::ofstream(block_path).close();

            if (truncate(block_path.c_str(), block_size_) != 0)
            {
                log(std::string("Couldn't truncate block: ") + std::to_string(block));
                return -1;
            }
        }

        return 0;
    }

    int format()
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        delete root_;

        root_ = new FD();
        root_->id = 0;
        strcpy(root_->name, "/");
        root_->is_dir = true;
        log("Root descriptor reset");

        std::fill(blocks_using_.begin(), blocks_using_.end(), false);
        log("Blocks using reset");

        return 0;
    }

    int imp(const std::string& src_path, const std::string& dest_path)
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        int64_t fd_id = get_file_descriptor(root_, root_path_, dest_path, FDT_FILE, true);

        if (fd_id < 0)
        {
            log("Couldn't get FD for import");
            return -1;
        }

        FD* dest_fd = read_file_descriptor(root_path_, (uint32_t) fd_id);

        std::ifstream src_ifstream(src_path, std::ios::binary);

        int result = imp(root_path_, src_ifstream, dest_fd, blocks_using_, block_data_size_);

        delete_fd_if_not_root(dest_fd);

        return result;
    }

    int exp(const std::string& src_path, const std::string& dest_path)
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        int64_t fd_id = get_file_descriptor(root_, root_path_, src_path, FDT_FILE, false);

        if (fd_id < 0)
        {
            log("Couldn't get FD for export");
            return -1;
        }

        FD* src_fd = read_file_descriptor(root_path_, (uint32_t) fd_id);

        std::ofstream dest_ofstream(dest_path, std::ios::binary);

        int result = exp(root_path_, src_fd, dest_ofstream, block_data_size_);

        delete_fd_if_not_root(src_fd);

        return result;
    }

    int ls(const std::string& path)
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        int64_t fd_id = get_file_descriptor(root_, root_path_, path, FDT_ANY, false);

        if (fd_id < 0)
        {
            log("Couldn't get FD for ls");
            return -1;
        }

        FD* fd = read_file_descriptor(root_path_, (uint32_t) fd_id);

        if (fd->is_dir)
        {
            int64_t child_id = fd->first_child_id;

            while (child_id > -1)
            {
                FD* child = read_file_descriptor(root_path_, (uint32_t) child_id);

                std::cout << child->name;

                std::cout << " ";

                if (child->is_dir)
                {
                    std::cout << "d";
                }
                else
                {
                    std::cout << "f";
                }

                std::cout << std::endl;

                child_id = child->next_id;

                delete_fd_if_not_root(child);
            }
        }
        else
        {
            int64_t bd_id = fd->first_block_id;
            uint32_t count = 0;

            while (bd_id > -1)
            {
                ++count;

                BD* bd = read_block_descriptor(root_path_, (uint32_t) bd_id);

                bd_id = bd->next;

                delete bd;
            }

            std::cout << path << " " << count << std::endl;
        }

        delete_fd_if_not_root(fd);

        return 0;
    }

    int mkdir(const std::string& path)
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        int64_t fd_id = get_file_descriptor(root_, root_path_, path, FDT_DIR, true);

        if (fd_id < 0)
        {
            log("Couldn't get FD for mkdir");
            return -1;
        }

        return 0;
    }

    int rm(const std::string& path)
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        int64_t fd_id = get_file_descriptor(root_, root_path_, path, FDT_ANY, false);

        if (fd_id == 0)
        {
            log("Attempt to remove root FD");
            return -1;
        }

        if (fd_id < 0)
        {
            log("Couldn't get FD for rm");
            return -1;
        }

        FD* fd = read_file_descriptor(root_path_, (uint32_t) fd_id);

        int result = rm(root_path_, fd, blocks_using_);

        delete_fd_if_not_root(fd);

        return result;
    }

    int move(const std::string& src_path, const std::string& dest_path)
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        int64_t src_id = get_file_descriptor(root_, root_path_, src_path, FDT_ANY, false);

        if (src_id == 0)
        {
            log("Attempt to move root FD");
            return -1;
        }

        if (src_id < 0)
        {
            log("Couldn't get src FD for move");
            return -1;
        }

        FD* src_fd = read_file_descriptor(root_path_, (uint32_t) src_id);

        int64_t dest_id = get_file_descriptor(root_, root_path_, dest_path, (src_fd->is_dir) ? FDT_DIR : FDT_FILE, true);

        if (dest_id < 0)
        {
            log("Couldn't get dest FD for move");

            delete_fd_if_not_root(src_fd);

            return -1;
        }

        FD* dest_fd = read_file_descriptor(root_path_, (uint32_t) dest_id);

        int result = move(root_path_, src_fd, dest_fd, blocks_using_, block_data_size_);

        delete_fd_if_not_root(src_fd);
        delete_fd_if_not_root(dest_fd);

        return result;
    }

    int copy(const std::string& src_path, const std::string& dest_path)
    {
        if (!init_)
        {
            log("FS is not initialized");
            return -1;
        }

        int64_t src_id = get_file_descriptor(root_, root_path_, src_path, FDT_ANY, false);

        if (src_id == 0)
        {
            log("Attempt to copy root FD");
            return -1;
        }

        if (src_id < 0)
        {
            log("Couldn't get src FD for copy");
            return -1;
        }

        FD* src_fd = read_file_descriptor(root_path_, (uint32_t) src_id);

        int64_t dest_id = get_file_descriptor(root_, root_path_, dest_path, (src_fd->is_dir) ? FDT_DIR : FDT_FILE, true);

        if (dest_id < 0)
        {
            log("Couldn't get dest FD for copy");

            delete_fd_if_not_root(src_fd);

            return -1;
        }

        FD* dest_fd = read_file_descriptor(root_path_, (uint32_t) dest_id);

        int result = copy(root_path_, src_fd, dest_fd, blocks_using_, block_data_size_);

        delete_fd_if_not_root(src_fd);
        delete_fd_if_not_root(dest_fd);

        return result;
    }

private:
    void log(const std::string& message)
    {
//        std::cout << message << std::endl;
    }

    void read_config(const std::string& root_path, uint32_t& block_size, uint32_t& blocks)
    {
        std::ifstream config_ifstream(root_path + "/config");
        config_ifstream >> block_size >> blocks;
    }

    bool zero_block_exists(const std::string& root_path)
    {
        std::ifstream block_ifstream(get_block_path(root_path, 0));
        return (bool) block_ifstream;
    }

    std::string get_block_path(const std::string& root_path, const uint32_t block)
    {
        return root_path + "/block" + std::to_string(block);
    }

    uint32_t calculate_service_blocks(const uint32_t block_size, const uint32_t blocks)
    {
        return (uint32_t) ceil((blocks / 8 + sizeof(FD)) / block_size);
    }

    // not pure
    FD* read_file_descriptor(const std::string& root_path, const uint32_t block)
    {
        if (block == 0 && root_ != nullptr) {
            return root_;
        }

        FD* fd = new FD();

        std::ifstream block_ifstream(get_block_path(root_path, block), std::ios::binary);
        block_ifstream.read((char*) fd, sizeof(FD));

        /*
        TODO rm
        std::cout << fd->parent_id << std::endl;
        std::cout << fd->first_child_id << std::endl;
        std::cout << fd->next_id << std::endl;
        std::cout << fd->prev_id << std::endl;
        std::cout << fd->first_block_id << std::endl;
        std::cout << fd->is_dir << std::endl;
        */

        log(std::string("FD loaded: ") + std::to_string(fd->id));

        return fd;
    }

    BD* read_block_descriptor(const std::string& root_path, const uint32_t block)
    {
        BD* bd = new BD();

        std::ifstream block_ifstream(get_block_path(root_path, block), std::ios::binary);
        block_ifstream.read((char*) bd, sizeof(BD));

        log(std::string("BD loaded:") + std::to_string(bd->id));

        return bd;
    }

    std::vector<bool> read_blocks_using(const std::string& root_path, const uint32_t available_blocks, const uint32_t block_size)
    {
        std::vector<bool> result(available_blocks);

        uint32_t current_service_block = 0;

        std::ifstream current_service_block_ifstream(get_block_path(root_path, current_service_block), std::ios::binary);
        current_service_block_ifstream.seekg(sizeof(FD));

        for (uint32_t block = 0; block < available_blocks; )
        {
            char b;
            current_service_block_ifstream >> b;

            for (uint32_t bit = 0; bit < 8 && block < available_blocks; ++bit, ++block)
            {
                result[block] = b & (1 << bit);
            }

            std::ios::streampos stream_pos = current_service_block_ifstream.tellg();

            if (stream_pos == block_size)
            {
                current_service_block_ifstream.close();
                current_service_block_ifstream.open(get_block_path(root_path, ++current_service_block), std::ios::binary);
            }
        }

        log("Blocks using loaded");

        return result;
    };

    void write_file_descriptor(const std::string& root_path, const FD* fd)
    {
        if (fd->id < 0)
        {
            log(std::string("Invalid block id: ") + std::to_string(fd->id));
            return;
        }

        std::ofstream block_ofstream(get_block_path(root_path, (uint32_t) fd->id), std::ios::binary);
        block_ofstream.write((char*) fd, sizeof(FD));

        /*
        TODO rm
        std::cout << fd->parent_id << std::endl;
        std::cout << fd->first_child_id << std::endl;
        std::cout << fd->next_id << std::endl;
        std::cout << fd->prev_id << std::endl;
        std::cout << fd->first_block_id << std::endl;
        std::cout << fd->is_dir << std::endl;
        */

        log(std::string("FD saved: ") + std::to_string(fd->id));
    }

    void write_block(const std::string& root_path, const BD* bd, const char* data)
    {
        if (bd->id < 0)
        {
            log(std::string("Invalid block id: ") + std::to_string(bd->id));
            return;
        }

        std::ofstream block_ofstream(get_block_path(root_path, (uint32_t) bd->id), std::ios::binary);
        block_ofstream.write((char*) bd, sizeof(BD));
        block_ofstream.write(data, (uint32_t) bd->len);

        log(std::string("Block saved: ") + std::to_string(bd->id) + " " + std::to_string(bd->len));
    }

    void write_main(const std::string& root_path, const FD* root, const std::vector<bool>& blocks_using, const uint32_t block_size)
    {
        if (root->id < 0)
        {
            log(std::string("Invalid block id: ") + std::to_string(root->id));
            return;
        }

        uint32_t current_service_block = 0;

        std::ofstream current_service_block_ofstream(get_block_path(root_path, current_service_block), std::ios::binary);

        current_service_block_ofstream.write((char*) root, sizeof(FD));
        log(std::string("FD saved: ") + std::to_string(root->id));

        for (uint32_t block = 0; block < blocks_using.size(); )
        {
            char b = 0;

            for (uint32_t bit = 0; bit < 8 && block < blocks_using.size(); ++bit, ++block)
            {
                b |= blocks_using[block] << bit;
            }

            current_service_block_ofstream << b;

            std::ios::streampos stream_pos = current_service_block_ofstream.tellp();

            if (stream_pos == block_size)
            {
                current_service_block_ofstream.close();
                current_service_block_ofstream.open(get_block_path(root_path, ++current_service_block), std::ios::binary);
            }
        }

        log("Blocks using saved");
    }

    int64_t get_file_descriptor(FD* root, const std::string& root_path, const std::string& path, const fd_type fdt, const bool create)
    {
        if (fdt == FDT_ANY && create)
        {
            log("Invalid FD flags");
            return -1;
        }

        std::vector<std::string> path_elements = extract_path_elements(path);

        if (!valid_path_elements(path_elements))
        {
            log(std::string("Invalid path: ") + path);
            return -1;
        }

        FD* current_parent = root;

        for (std::vector<std::string>::const_iterator it = path_elements.begin(); it != path_elements.end(); ++it)
        {
            fd_type current_fdt = (it != path_elements.end() - 1 ? FDT_DIR : fdt);
            int64_t child_id = find_child_id(root_path, current_parent, *it, current_fdt);

            if (child_id < 0)
            {
                if (!create)
                {
                    log(std::string("Path doesnt exist:") + path);
                    return -1;
                }

                int64_t new_child_id = add_new_child_to_parent(root_path, current_parent, *it, current_fdt);

                if (new_child_id < 0)
                {
                    log(std::string("Couldn't create FD: ") + *it);
                    return -1;
                }
                else
                {
                    delete_fd_if_not_root(current_parent);

                    current_parent = read_file_descriptor(root_path, (uint32_t) new_child_id);
                }
            }
            else
            {
                delete_fd_if_not_root(current_parent);

                current_parent = read_file_descriptor(root_path, (uint32_t) child_id);
            }
        }

        int64_t result = current_parent->id;

        delete_fd_if_not_root(current_parent);

        return result;
    }

    int64_t find_child_id(const std::string& root_path, const FD* parent_fd, const std::string& expected_name, const fd_type fdt)
    {
        if (parent_fd->first_child_id > -1)
        {
            FD* current_child_fd = read_file_descriptor(root_path, (uint32_t) parent_fd->first_child_id);

            while (true) {
                if (strcmp(current_child_fd->name, expected_name.c_str()) == 0)
                {
                    bool is_dir = current_child_fd->is_dir;

                    if (fdt == FDT_ANY || (is_dir && fdt == FDT_DIR) || (!is_dir && fdt == FDT_FILE))
                    {
                        int64_t result = current_child_fd->id;

                        delete_fd_if_not_root(current_child_fd);

                        return result;
                    }
                }
                else if (current_child_fd->next_id != -1)
                {
                    delete_fd_if_not_root(current_child_fd);

                    current_child_fd = read_file_descriptor(root_path, (uint32_t) current_child_fd->next_id);
                }
                else
                {
                    delete_fd_if_not_root(current_child_fd);

                    return -1;
                }
            }
        }

        return -1;
    }

    int64_t add_new_child_to_parent(const std::string& root_path, FD* parent, const std::string& child_name, const fd_type fdt) {
        FD* child = new FD();

        child->id = find_free_block();

        if (child->id < 0)
        {
            log("Couldn't allocate block");
            return -1;
        }

        child->is_dir = (fdt == FDT_DIR);
        strcpy(child->name, child_name.c_str());
        child->parent_id = parent->id;

        if (parent->first_child_id < 0)
        {
            parent->first_child_id = child->id;
        }
        else
        {
            FD* prev_first_child = read_file_descriptor(root_path, (uint32_t) parent->first_child_id);

            prev_first_child->prev_id = child->id;
            child->next_id = prev_first_child->id;

            parent->first_child_id = child->id;

            write_file_descriptor(root_path, prev_first_child);

            delete_fd_if_not_root(prev_first_child);
        }

        write_file_descriptor(root_path, child);
        write_file_descriptor(root_path, parent);

        int64_t result = child->id;

        delete_fd_if_not_root(child);

        return result;
    }

    // not pure
    void delete_fd_if_not_root(FD* fd)
    {
        if (fd != root_)
        {
            delete fd;
        }
    }

    // not pure
    int64_t find_free_block() {
        for (size_t block = 0; block < blocks_using_.size(); ++block)
        {
            if (!blocks_using_[block])
            {
                blocks_using_[block] = true;
                return block + service_blocks_ + 1;
            }
        }

        return -1;
    }

    std::vector<std::string> extract_path_elements(const std::string& path)
    {
        std::stringstream path_stream(path.substr(1, path.size())); // remove leading '/'
        std::string element;

        std::vector<std::string> result;

        while (std::getline(path_stream, element, '/'))
        {
            result.push_back(element);
        }

        return result;
    }

    bool valid_path_elements(const std::vector<std::string>& path_elements)
    {
        for (std::vector<std::string>::const_iterator it = path_elements.begin(); it != path_elements.end(); ++it)
        {
            if ((*it).size() > 10) {
                return false;
            }
        }

        return true;
    }

    int imp(const std::string& root_path, std::ifstream& src_ifstream, FD* dest_fd, std::vector<bool>& blocks_using, const uint32_t block_data_size)
    {
        free_fd(root_path, dest_fd, blocks_using);

        int64_t block_id = find_free_block();

        if (block_id < 0)
        {
            log("Couldn't allocate block");
            return -1;
        }
        else
        {
            char* buffer = new char[block_data_size];

            dest_fd->first_block_id = block_id;

            while (src_ifstream.good())
            {
                src_ifstream.read(buffer, block_data_size);

                BD* bd = new BD();
                bd->id = block_id;
                bd->len = src_ifstream.gcount();

                if (!src_ifstream.good())
                {
                    bd->next = -1;
                }
                else
                {
                    block_id = find_free_block();

                    if (block_id < 0) {
                        log("Couldn't allocate block");

                        free_fd(root_path, dest_fd, blocks_using);
                        delete bd;
                        delete[] buffer;

                        return -1;
                    }
                    else
                    {
                        bd->next = block_id;
                    }
                }

                write_block(root_path, bd, buffer);

                delete bd;
            }

            write_file_descriptor(root_path, dest_fd);

            delete[] buffer;

            return 0;
        }
    }

    int exp(const std::string& root_path, const FD* src_fd, std::ofstream& dest_ofstream, const uint32_t block_data_size)
    {
        char* buffer = new char[block_data_size];
        int64_t next = src_fd->first_block_id;

        while (next != -1)
        {
            BD* bd = read_block_descriptor(root_path, (uint32_t) next);

            std::ifstream block_ifstream(get_block_path(root_path, (uint32_t) next), std::ios::binary);
            block_ifstream.seekg(sizeof(BD));
            block_ifstream.read(buffer, bd->len);
            block_ifstream.close();

            dest_ofstream.write(buffer, bd->len);
            next = bd->next;

            log(std::string("BD exported: ") + std::to_string(bd->id));

            delete bd;
        }

        delete[] buffer;

        return 0;
    }

    int rm(const std::string& root_path, FD* fd, std::vector<bool>& blocks_using)
    {
        if (fd->is_dir)
        {
            if (fd->first_child_id > -1)
            {
                int64_t child_id = fd->first_child_id;

                while (true)
                {
                    FD* child_fd = read_file_descriptor(root_path, (uint32_t) child_id);

                    int result = rm(root_path, child_fd, blocks_using);

                    child_id = child_fd->next_id;

                    delete_fd_if_not_root(child_fd);

                    if (result < 0)
                    {
                        return result;
                    }

                    if (child_id < 0)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            free_fd(root_path, fd, blocks_using);
        }

        return remove_fd(root_path, fd, blocks_using);
    }

    void free_fd(const std::string& root_path, FD* fd, std::vector<bool>& blocks_using)
    {
        if (fd->first_block_id > -1)
        {
            BD* bd = read_block_descriptor(root_path, (uint32_t) fd->first_block_id);

            fd->first_block_id = -1;
            write_file_descriptor(root_path, fd);

            while (true) {
                blocks_using[bd->id] = false;

                if (bd->next < 0)
                {
                    log(std::string("FD cleaned: ") + std::to_string(fd->id));

                    delete bd;

                    return;
                }
                else
                {
                    delete bd;

                    bd = read_block_descriptor(root_path, (uint32_t) bd->next);
                }
            }
        }
    }

    int remove_fd(const std::string& root_path, FD* fd, std::vector<bool>& blocks_using)
    {
        if (fd->parent_id < 0)
        {
            log("Attempt to remove root FD");
            return -1;
        }

        if (fd->prev_id < 0)
        {
            if (fd->next_id < 0) {
                FD* parent_fd = read_file_descriptor(root_path, (uint32_t) fd->parent_id);

                parent_fd->first_child_id = -1;

                write_file_descriptor(root_path, parent_fd);

                delete_fd_if_not_root(parent_fd);
            }
            else
            {
                FD* parent_fd = read_file_descriptor(root_path, (uint32_t) fd->parent_id);
                FD* next_fd = read_file_descriptor(root_path, (uint32_t) fd->next_id);

                parent_fd->first_child_id = next_fd->id;
                next_fd->parent_id = parent_fd->id;
                next_fd->prev_id = -1;

                write_file_descriptor(root_path, parent_fd);
                write_file_descriptor(root_path, next_fd);

                delete_fd_if_not_root(parent_fd);
                delete_fd_if_not_root(next_fd);
            }
        }
        else
        {
            if (fd->next_id < 0)
            {
                FD* prev_fd = read_file_descriptor(root_path, (uint32_t) fd->prev_id);

                prev_fd->next_id = -1;

                write_file_descriptor(root_path, prev_fd);

                delete_fd_if_not_root(prev_fd);
            }
            else
            {
                FD* prev_fd = read_file_descriptor(root_path, (uint32_t) fd->prev_id);
                FD* next_fd = read_file_descriptor(root_path, (uint32_t) fd->next_id);

                prev_fd->next_id = next_fd->id;
                next_fd->prev_id = prev_fd->id;

                write_file_descriptor(root_path, prev_fd);
                write_file_descriptor(root_path, next_fd);

                delete_fd_if_not_root(prev_fd);
                delete_fd_if_not_root(next_fd);
            }
        }

        blocks_using[fd->id] = false;

        return 0;
    }

    int copy(const std::string& root_path, const FD* src_fd, FD* dest_fd, std::vector<bool>& blocks_using, const uint32_t block_data_size)
    {
        if (dest_fd->is_dir && src_fd->is_dir)
        {
            return copy_dir_to_dir(root_path, src_fd, dest_fd, blocks_using, block_data_size);
        }
        else if (!dest_fd->is_dir && !src_fd->is_dir)
        {
            return copy_file_to_file(root_path, src_fd, dest_fd, blocks_using, block_data_size);
        }
        else
        {
            log("Unsupported copy");
            return -1;
        }
    }

    int copy_file_to_file(const std::string& root_path, const FD* src_fd, FD* dest_fd, std::vector<bool>& blocks_using, const uint32_t block_data_size) {
        int64_t dest_block = find_free_block();

        if (dest_block < 0)
        {
            log("Couldn't allocate block");
            return -1;
        }

        free_fd(root_path, dest_fd, blocks_using);

        int64_t src_block = src_fd->first_block_id;

        char* buffer = new char[block_data_size];

        dest_fd->first_block_id = dest_block;

        while (true)
        {
            BD* bd = read_block_descriptor(root_path, (uint32_t) src_block);

            std::ifstream block_ifstream(get_block_path(root_path, (uint32_t) src_block), std::ios_base::binary);
            block_ifstream.seekg(sizeof(BD));
            block_ifstream.read(buffer, bd->len);
            block_ifstream.close();

            BD* new_bd = bd;
            new_bd->id = dest_block;

            src_block = bd->next;

            if (src_block == -1)
            {
                new_bd->next = -1;
            }
            else
            {
                dest_block = find_free_block();

                if (dest_block < 0)
                {
                    log("Couldn't allocate block");

                    delete bd;
                    delete[] buffer;

                    return -1;
                }

                new_bd->next = dest_block;
            }

            write_block(root_path, new_bd, buffer);

            delete bd;

            if (src_block < 0)
            {
                break;
            }
        }

        delete[] buffer;

        write_file_descriptor(root_path, dest_fd);

        return 0;
    }

    int copy_dir_to_dir(const std::string& root_path, const FD* src_fd, FD* dest_fd, std::vector<bool>& blocks_using, const uint32_t block_data_size) {
        if (src_fd->first_child_id > -1)
        {
            int64_t child_id = src_fd->first_child_id;

            while (true)
            {
                FD* child_fd = read_file_descriptor(root_path, (uint32_t) child_id);

                int64_t new_id = get_file_descriptor(dest_fd, root_path, child_fd->name, (child_fd->is_dir) ? FDT_DIR : FDT_FILE, true);

                if (new_id < 0)
                {
                    log("Couldn't get dest FD for copy");

                    delete_fd_if_not_root(child_fd);

                    return -1;
                }

                FD* new_fd = read_file_descriptor(root_path, (uint32_t) new_id);

                int result = copy(root_path, child_fd, new_fd, blocks_using, block_data_size);

                child_id = child_fd->next_id;

                delete_fd_if_not_root(child_fd);
                delete_fd_if_not_root(new_fd);

                if (result < 0)
                {
                    return result;
                }

                if (child_id < 0)
                {
                    break;
                }
            }
        }

        return 0;
    }

    int move(const std::string& root_path, FD* src_fd, FD* dest_fd, std::vector<bool>& blocks_using, const uint32_t block_data_size)
    {
        if (dest_fd->is_dir && src_fd->is_dir)
        {
            if (src_fd->first_child_id > -1)
            {
                FD* first_child_fd = read_file_descriptor(root_path, (uint32_t) src_fd->first_child_id);

                if (dest_fd->first_child_id < 0)
                {
                    dest_fd->first_child_id = first_child_fd->id;
                    first_child_fd->parent_id = dest_fd->id;

                    write_file_descriptor(root_path, dest_fd);
                }
                else
                {
                    FD* child_fd = read_file_descriptor(root_path, (uint32_t) dest_fd->first_child_id);

                    while (child_fd->next_id > -1)
                    {
                        child_fd = read_file_descriptor(root_path, (uint32_t) child_fd->next_id);
                    }

                    child_fd->next_id = first_child_fd->id;
                    first_child_fd->prev_id = child_fd->id;

                    write_file_descriptor(root_path, child_fd);
                }

                write_file_descriptor(root_path, first_child_fd);
            }

            remove_fd(root_path, src_fd, blocks_using);

            return 0;
        }
        else if (!dest_fd->is_dir && !src_fd->is_dir)
        {
            free_fd(root_path, dest_fd, blocks_using);

            dest_fd->first_block_id = src_fd->first_block_id;

            write_file_descriptor(root_path, dest_fd);

            remove_fd(root_path, src_fd, blocks_using);

            return 0;
        }
        else
        {
            log("Unsupported move");
            return -1;
        }
    }

};