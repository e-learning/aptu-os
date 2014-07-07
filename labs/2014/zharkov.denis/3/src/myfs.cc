#include <fstream>
#include <cassert>
#include <cmath>
#include <algorithm>

using namespace std;

#include "myfs.h"

MyFileSystem * MyFileSystem::instance_ = nullptr;

MyFileSystem::~MyFileSystem() {
    delete free_blocks_;
}

void MyFileSystem::load_config() {
    string config_file_name = root_path_ + HOST_FS_DELIMITER + CONFIG_FILE_NAME;
    
    ifstream config(config_file_name);

    config >> block_size_ >> blocks_count_;

    free_blocks_ = new BlockList(free_blocks_first);
}

void MyFileSystem::format() {
    BlockListBlock list_block(load_block(free_blocks_first));
    list_block.clear();
    
    size_t free_blocks_count = blocks_count_ - data_blocks_starts;
    size_t slots_per_block = list_block.slots_count();
    size_t helper_blocks_count = free_blocks_count > slots_per_block ? (size_t)(ceil((free_blocks_count - slots_per_block) / (slots_per_block + 1.0))) : 0;
    
    block_id_t helper_blocks_starts = data_blocks_starts;
    block_id_t helper_blocks_ends = data_blocks_starts + helper_blocks_count;

    for (block_id_t block = helper_blocks_ends; block < blocks_count_; ++block) {
        if (list_block.full()) {
            block_id_t next = helper_blocks_starts++;

            assert(next < helper_blocks_ends); 

            list_block.set_next(next);

            write_block(*(list_block.block_));
            list_block = BlockListBlock(load_block(next));
            list_block.clear();
        }

        list_block.add(block);
    }
    write_block(*(list_block.block_));
    
    BlockPtr meta = load_block(metainfo_block);
    meta->put_int(0, time(NULL));
    write_block(*meta);

    root().init();
}

void MyFileSystem::init() {
    for (size_t i = 0; i < blocks_count_; i++) {
        ofstream block_stream(block_file_name(i), ios_base::binary);
        
        for (size_t j = 0; j < block_size_; j++) {
            block_stream.put(0);
        }

        block_stream.close();
    }
}

block_id_t MyFileSystem::acquire_free_block() {
    return free_blocks_->pop();
}

void MyFileSystem::release_block(block_id_t id) {
    return free_blocks_->add(id);
}

FileList MyFileSystem::root() {
    return FileList(root_dir_block);
}

File MyFileSystem::go_through_path(const vector<string> & path, bool expect_dir) {
    File file = root_directory();
    
    for (string const & next_name : path) {
        if (!file.is_dir) {
            throw FsException(file.name + " is not dir");
        }

        FileList list(file.block_id);

        auto next = list.find(next_name);
        
        if (next == list.end()) {
            throw FsException("file doesn't exists: " + next_name);
        }

        file = *next;
    }

    if (expect_dir && !file.is_dir) {
        throw FsException(file.name + " is not dir");
    }

    return file;
}
size_t MyFileSystem::free_blocks() {
    if (free_blocks_cnt_ == 0) {
        BlockList free(free_blocks_first);

        int cnt = 0;
        for_each(free.begin(), free.end(), [&cnt](block_id_t) {
            cnt++;
        });

        free_blocks_cnt_ = cnt;
    }

    return free_blocks_cnt_;
}

vector<string> split_path(string path) {
    vector<string> result;
    
    path = path.substr(1);
    size_t next;
    while ((next = path.find("/")) != string::npos) {
        result.push_back(path.substr(0, next));
        path = path.substr(next + 1);
    }
    
    if (path.length() > 0) {
        result.push_back(path);
    }

    return result;
}
