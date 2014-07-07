#ifndef FILEDESC_H
#define FILEDESC_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "time.h"
#include <cstring>
#include <stdexcept>
#include <sstream>

using std::string;
using std::vector;
using std::stringstream;


struct Block {
    int id;
    int next_id;
    size_t length;
};


string to_string(int id, string const & root);

Block read_block_desc(int id, string const & root);


struct FileDesc;

string to_string(FileDesc const & f, string const & root);

FileDesc read_desc(int id, string const & root);

void write_desc(FileDesc const & d, string const & root, vector<bool> & bitmap, FileDesc & root_desc);


struct FileDesc {

    FileDesc() :
        id(-1), is_directory(false), parent_id(-1),
        prev_file_id(-1), next_file_id(-1), first_file_id(-1),
        first_block(-1), block_count(0), modified_time(time(0)) {
        strncpy(name, "", sizeof(name));
    }

    FileDesc(string const & name, bool is_dir) :
        id(-1), is_directory(is_dir), parent_id(-1),
        prev_file_id(-1), next_file_id(-1), first_file_id(-1),
        first_block(-1), block_count(0), modified_time(time(0)) {
        if (name.size() <= 10) {
            strncpy(this->name, name.c_str(), sizeof(this->name));
        } else {
            throw std::runtime_error("Uncorrect name");
        }


    }

    FileDesc(FileDesc const & other) :
        id(other.id), is_directory(other.is_directory), parent_id(other.parent_id),
        prev_file_id(other.prev_file_id), next_file_id(other.next_file_id), first_file_id(other.first_file_id),
        first_block(other.first_block), block_count(other.block_count), modified_time(other.modified_time) {

        strncpy(this->name, other.name, sizeof(this->name));
    }

    FileDesc & operator = (FileDesc const & other) {
        if (&other != this) {
            FileDesc(other).swap(*this);
        }
        return *this;
    }

    string get_info();

    void clear(vector<bool> & bitmap, string const & root);

    void remove(vector<bool> & bitmap, string const & root, FileDesc & root_desc);

    char name[10];
    int id;
    bool is_directory;
    int parent_id;
    int prev_file_id;
    int next_file_id;
    int first_file_id;
    int first_block;
    size_t block_count;
    time_t modified_time;

private:

    void swap(FileDesc & other) {

        std::swap(name, other.name);
        std::swap(id, other.id);
        std::swap(is_directory, other.is_directory);
        std::swap(parent_id, other.parent_id);
        std::swap(prev_file_id, other.prev_file_id);
        std::swap(next_file_id, other.next_file_id);
        std::swap(first_file_id, other.first_file_id);
        std::swap(first_block, other.first_block);
        std::swap(block_count, other.block_count);
        std::swap(modified_time, other.modified_time);


    }

};



#endif // FILEDESC_H
