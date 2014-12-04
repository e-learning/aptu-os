#pragma once

#include <string>
#include <time.h>
#include <vector>
#include <iostream>
#include <iterator>
#include "util.h"

class dir_iterator;

class file_system
{

public:
    file_system(std::string root);

    ~file_system();

    void init();

    void move(std::string src, std::string dst);

    void copy(std::string src, std::string dest);

    void file_export(std::string filesystem_file, std::string host_file);

    void format();

    void ls(std::string path);

    void mkdir(std::string path);

    void rm(std::string path);

    void import(std::string host_file, std::string filesystem_file);


private:
    void copy(file_descriptor src, file_descriptor &dest_file, std::string dest_filename);

    void rm(file_descriptor file);

    filesystem_config read_config();

    meta_inf meta;
    bool initialized;
    std::string _root;
    const filesystem_config config;

    std::string get_block_name(int block_id);

    file_descriptor get_file(std::string path, bool create, bool file_available);

    void free_block(int n);

    void reserve_block(int n);

    void write_data(void *data, int size, block *first_block);

    void read_data(void *data, int size, block *first_block);

    void write_metainformation();

    void read_metainformation();

    void update_descriptor(file_descriptor &fd, block *block);

    block *get_free_block();

    friend class dir_iterator;
};

class dir_iterator : std::iterator<std::forward_iterator_tag, file_descriptor>
{
public:
    dir_iterator(file_system &fs) :
            _fs(fs)
    {
        p = 0;
    }

    dir_iterator(file_system &fs, file_descriptor dir);

    dir_iterator &operator++();

    dir_iterator operator++(int)
    {
        dir_iterator tmp(*this);
        operator++();
        return tmp;
    }

    bool operator==(const dir_iterator &rhs)
    {
        return p->filename == rhs.p->filename;
    }

    bool operator!=(const dir_iterator &rhs)
    {
        return p->filename != rhs.p->filename;
    }

    file_descriptor &operator*()
    {
        if (p == 0) throw ("Call * of zero iterator");
        return *p;
    }

private:
    file_descriptor *p;
    file_system &_fs;
};

