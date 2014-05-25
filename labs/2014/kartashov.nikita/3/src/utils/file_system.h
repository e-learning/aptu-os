#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "data.h"
#include <vector>
#include <fstream>

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;


class file_system 
{
public:
    file_system(const char *root);
    ~file_system();

    void format();
    void rm(const file_record &fd);
    void copy(const file_record &, file_record &);
    void move(const file_record &, file_record &);
    vector<file_record> list(const file_record & directory) const;

    file_record find_descriptor(const char *destination,  bool create = true, bool is_directory = false);
    file_record find_directory(const char *destination, bool create = true);

    void read_data(const file_record &, std::ostream &);
    void write_data(file_record &, std::istream &);

    Config get_config();

    template <typename D = file_record>
    D read_descriptor(int id) const 
		{
        D fd;
        ifstream block(get_block_f(id), ios::binary);
        block.read((char *) &fd, sizeof(D));
        if (block)
          return fd;
        else
					throw "Cannot read descriptor ";
    }

    void write_descriptor(file_record const& d) 
		{
        if (d.id == 0)
            root_d = d;
        ofstream block(get_block_f(d.id), ios::binary);
        block.write((char*) &d, sizeof(file_record));
        if (!block)
            throw "Cannot write descriptor";
        block_map[d.id] = true;
    }

    void check_initialized()
		{
        if (!initialized) 
            throw "Filesystem is not initialized";
    }

    const string get_block_f(const int id) const 
		{
        return string(root) + "/" + std::to_string(id);
    }

private:
    const char *root;
    const Config config;
    const int BLOCK_DATA_SIZE;
    const bool initialized;
    file_record root_d;
    vector<bool> block_map;

    file_record init_descriptor(const string &name, bool directory = true);
    void clear_descriptor(const file_record &fd);
    void remove_descriptor(const int);

    file_record find_descriptor(file_record & directory, const string & name, bool create = true, bool is_directory = false);
    file_record find_directory(const vector<string> & path, bool create = true);

    void read_block_map();
    void write_block_map();

    size_t find_first_free_block(const int start = 0) const 
		{
        for (size_t i = start + 1; i < block_map.size(); i++)
            if (!block_map[i]) 
                return i;
        throw "Not enough volume";
    }
};

#endif
