#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <vector>
#include <fstream>
#include <time.h>
#include <stdexcept>

#include "filedesc.h"

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::runtime_error;


struct Config {
    int block_size;
    int block_count;
};



class FileSystem {

public:

    FileSystem(string const & root);
    ~FileSystem();

    void init_cmd();
    void format_cmd();
    void import_cmd(string const & sourse_file_name, string const & target_file_name);
    void export_cmd(string const & sourse_file_name, string const & target_file_name);
    void ls_cmd(string const & file_name);
    void rm_cmd(string const & file_name);
    void mkdir_cmd(string const & dir_name);
    void copy_cmd(string const & sourse_file_name, string const & target_file_name);
    void move_cmd(string const & sourse_file_name, string const & target_file_name);

    Config get_config();

    void read_data(FileDesc const &, std::ostream &);
    void write_data(FileDesc &, std::istream &);

    FileDesc find_desc(string const & dest,  bool create, bool is_dir);
    FileDesc find_dir(string const & dest, bool create) {
        return find_dir(get_path(dest), create);
    }

    vector<string> get_path(string const & name);

private:

    string root;
    FileDesc root_desc;
    static const string CONFIG_FILENAME;
    Config config;
    int block_data_size;
    bool is_init;
    vector<bool> bitmap;

    void read_bitmap();
    void write_bitmap();


    FileDesc find_desc(FileDesc & dir, string const & name_desc, bool create, bool is_dir);
    FileDesc find_dir(vector<string> const & path, bool create);
    vector<FileDesc> get_list_child(FileDesc const & dir);
    void rm(FileDesc & fd);

    void copy(FileDesc const & source_desc, FileDesc & target_desc);
    void move(FileDesc & source_desc, FileDesc & target_desc);

    size_t find_first_free_block() {
        for (size_t i = 1; i < bitmap.size(); ++i){
            if (!bitmap[i]) {
                return i;
            }
        }
        throw runtime_error("Not enough space");
    }
};

#endif // FILESYSTEM_H
