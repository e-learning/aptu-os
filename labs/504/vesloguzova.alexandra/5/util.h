#pragma once


#include <ctime>
#include <string>
#include <cstring>
#include <stdexcept>

#define MAX_NAME_LENGTH 11

struct meta_info
{
    int root_block;
    int block_map_size;
    char *block_map;
};

struct block_desc
{
    int next = -1;
    int data_written = 0;
};


class block_t
{
public:
    block_desc _descriptor;

    block_t(int index, int size);

    block_t(int i, int size, std::string root);

    int write(void *data, int size);

    int read(void *data, int size);

    void move_to_begin()
    {
        _descriptor.data_written = 0;
    }

    char *get_data()
    {
        return _data;
    }

    const int get_size() const
    {
        return size;
    }

    const int get_index() const
    {
        return _index;
    }

    void save_block(std::string root);

    bool is_full();

    ~block_t();

private:
    void load_block(std::string root);

    int size;
    int pos = 0;
    int _index;
    char *_data;
};

struct config_info
{
    int block_size;
    int block_no;
};

struct file_descriptor
{
    int id = -1;
    bool directory = true;
    int first_child = -1;
    int parent_file = -1;
    int next_file = -1;
    int prev_file = -1;
    int size = 0;
    int first_block = -1;
    char filename[MAX_NAME_LENGTH];
    time_t last_update = time(0);
    void set_filename(std::string name)
    {
        for (int i = 0; i < MAX_NAME_LENGTH; ++i)
        {
            filename[i] = 0;
        }
        strncpy(filename, name.c_str(), MAX_NAME_LENGTH - 1);
    }
};


