#pragma once

#include <iostream>

#include <string>
#include <fstream>
#include <exception>
#include <vector>
#include <iterator>
#include <tuple>

#include "directory_t.h"

using namespace std;

class out_file_buffer;
class in_file_buffer;

class fs_t {
public:
    class fs_error: public runtime_error {
    public:
        explicit fs_error(const string&);
    };

    explicit fs_t(const string &path, bool pass_metadata = false);
    ~fs_t();

    bool is_valid() const;
    bool is_formatted() const;

    void init();
    void format();
    void import_(const string&, const string&);
    void export_(const string&, const string&);
    string ls(const string&);
    void copy(const string&, const string&);
    void move(const string&, const string&);
    void rm(const string&);
    void mkdir(const string&);

private:
    friend class out_file_buffer;
    friend class in_file_buffer;

    const string CONFIG_FILENAME   = "config";

    static const uint32_t NO_FREE_BLOCKS = -1;
    static const size_t MIN_BLOCK_SIZE = 1024;

    static const char FREE = 0x00;
    static const char USED = 0x01;

    string path_;
    size_t block_size_;
    size_t blocks_count_;

    directory_t root_;
    vector<char> bitmap_;

    size_t block_size() const;
    string block_path(uint32_t index);

    uint32_t next_free_block() const;
    void mark_block(uint32_t index, char value);

    directory_t* find_last(vector<string>);
    tuple<directory_t*, directory_t*, string, string> find_roots(const string&, const string&);

    void copy_directory(directory_t*, const string&, directory_t*, directory_t*);
    void copy_file(file_t*, const string&, directory_t*);

    void remove_file(file_t*);
    void remove_directory(directory_t*);

    void read_config_file();

    void read_metadata_file();
    void read_bitmap_file();

    void write_metadata_file();
    void write_bitmap_file();
};
