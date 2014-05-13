#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <memory>
#include <exception>
#include <cmath>
using namespace std;

#ifndef MY_FILE_SYSTEM_H
#define MY_FILE_SYSTEM_H

typedef size_t block_id_t;

char const * const CONFIG_FILE_NAME = "config";
char const * const HOST_FS_DELIMITER = "/";
char const * const FS_DELIMITER = "/";

const block_id_t metainfo_block = 0;
const block_id_t root_dir_block = 1;
const block_id_t free_blocks_first = 2;
const block_id_t data_blocks_starts = 3;

const size_t max_filename_length = 10;

struct FsException : exception {
    FsException(string error)
        : error_(error)
    {

    }

    const char* what() const noexcept (true) {
        return error_.c_str();
    }

    string error_;
};

struct Block {
    
    Block(block_id_t id, size_t size) 
        : size_(size), id_(id)
    {
        data_ = new char[size];
    }

    ~Block() {
        delete [] data_;
    }

    char & byte(size_t offset) {
        return data_[offset];
    }
    
    template<typename T>
    T get_int(size_t offset) {
        T value;
        memcpy(&value, data_ + offset, sizeof(T));
        return value;
    }

    template<typename T>
    void put_int(size_t offset, T value) {
        memcpy(data_ + offset, &value, sizeof(T));
    }

    string get_string(size_t offset, size_t max_len) {
        return string(data_ + offset, min(max_len, strlen(data_ + offset)));
    }

    void put_string(size_t offset, string const & str, size_t max_len) {
        memcpy(data_ + offset, str.c_str(), min(max_len, str.length() + 1));
    }

    void swap_data(Block & other) {
        char * tmp = data_;
        data_ = other.data_;
        other.data_ = tmp;
    }

    void copy_from(Block const & other) {
        copy(other.data_, other.data_ + size_, data_);
    }
    
    void clear_data() {
        fill(data_, data_ + size_, 0);
    }

    size_t size_;
    block_id_t id_;
    char * data_;
};

struct File {
    block_id_t block_id;
    string name;
    bool is_dir;
    size_t size;
    time_t last_modification_time;
};

typedef unique_ptr<Block> BlockPtr;

class BlockList;
class FileList;

struct MyFileSystem {
    MyFileSystem() {

    }
    MyFileSystem(const string & root_path)
        : root_path_(root_path) 
    {
       load_config();
       instance_ = this;
       free_blocks_cnt_ = 0;
    }

    ~MyFileSystem();

    static MyFileSystem & instance() {
        return *instance_;
    }

    void init();
    void format();
    
    BlockPtr load_block(block_id_t id) {
        BlockPtr block(new Block(id, block_size_));

        ifstream block_file(block_file_name(id), ios_base::binary);
        block_file.read(block->data_, block_size_);
        
        return block;
    }

    block_id_t acquire_free_block();
    void release_block(block_id_t);

    void write_block(Block & block) {
        ofstream block_file(block_file_name(block.id_), ios_base::binary);
        block_file.write(block.data_, block_size_);
    }
    
    time_t get_fs_created_at() {
        return load_block(metainfo_block)->get_int<time_t>(0);
    }

    File const & root_directory() {
        static File root({root_dir_block, FS_DELIMITER, true, 0, get_fs_created_at()});
        return root;
    }

    size_t block_size() {
        return block_size_;
    }
    
    size_t blocks_count() {
        return blocks_count_;
    }

    void check_if_space_available(size_t size) {
        size_t blocks_need = ceil(1.1 * size / block_size_);

        if (blocks_need > free_blocks()) {
            throw FsException("no free space");
        }
    }

    size_t free_blocks();
    
    File go_through_path(const vector<string> & path, bool expect_dir = false);

    FileList root();

    File new_file(string const & filename, bool is_dir, size_t size = 0) {
        File res({acquire_free_block(), filename, is_dir, size, time(NULL)});
        load_block(res.block_id)->clear_data();
        return res;
    }

private:
     
    void load_config();
    
    string block_file_name(block_id_t block_id) const {
        return root_path_ + HOST_FS_DELIMITER + to_string(block_id);    
    }

    const string root_path_; 
    size_t block_size_;
    size_t blocks_count_;
    size_t free_blocks_cnt_;
    
    BlockList * free_blocks_;

    static MyFileSystem * instance_; 
};

vector<string> split_path(string path);
inline pair<vector<string>, string> split_basename(string path) {
    auto splitted = split_path(path);
    auto basename = splitted.back();
    splitted.pop_back();

    return make_pair(splitted, basename);
}

#include "file_list.h"
#include "block_list.h"

#endif

