#pragma once
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <exception>

using namespace std;


class FSException : public exception
{
private:
    string msg;
public:
    FSException() { }
    FSException(const string mes)
        : msg(mes)
    { }
    ~FSException() throw() { };
    const char* what() const throw()
    {
        return ("Error: " + msg).c_str();
    }
};

class Inode
{
private:
    static const int MAX_FILENAME_LENGTH = 10;
public:
    int32_t id;
    bool is_dir;
    char name[MAX_FILENAME_LENGTH];
    size_t blocks_num;
    int32_t first_block;
    int32_t first_child;
    int32_t parent_inode;
    int32_t prev_inode;
    int32_t next_inode;
};

struct Block
{
    int32_t id;
    int32_t next;
    size_t length;
};

struct Config
{
    int block_size;
    int blocks_num;
};


class Meta_FS
{
private:
    const char *root;
    const Config config;
    const int block_size;
    const bool is_init;
    Inode root_dir;
    vector<bool> is_free_block;
    Inode init_inode(const string &name, bool is_dir = true);
    void clear_inode(const Inode &inode);
    void remove_inode(const int id);
    Inode get_inode(Inode &directory, const string &inode_name, bool create = true, bool is_directory = false);
    Inode get_directory(const vector<string> &path, bool create = true);
    size_t get_available_block(const int start = 0) const;
    void read_block_map();
    void write_block_map();
public:
    Meta_FS(const char *root);
    ~Meta_FS();
    Config get_config();
    string get_block_path(const int32_t id) const;
    void format();
    void cp(const Inode &, Inode &);
    void mv(const Inode &, Inode &);
    void rm(const Inode &inode);
    vector<Inode> ls(const Inode & directory) const;
    void check_availability(ifstream & in) const;
    void check_availability(size_t req_blocks_num) const;
    Inode get_inode(const char *dst, bool create = true, bool is_dir = false);
    Inode get_directory(const char *dst, bool create = true);
    void read_data(Inode const &, ostream &);
    void write_data(Inode &, istream &);
    Inode get_inode_by_id(int32_t id) const;
    Block read_block(int32_t id) const;
    void write_inode(const Inode &inode);
};
template<typename T>
ostream & operator<<(ostream &output, const vector<T> & v);
ostream & operator <<(ostream &output, const Inode &inode);
