#ifndef MYFS_H
#define MYFS_H

#include <string>
#include <fstream>
#include <iostream>
#include <ios>
#include <vector>
#include <stdlib.h>
#include <cinttypes>
#include <cstdint>
#include <algorithm>
#include <string.h>
#define MAX_DISK_SIZE 5242880
#define MIN_BLOCK_SIZE 1024

struct MyFile
{
    int32_t     fd;
    int32_t     next_file;
    int32_t     prev_file;
    int32_t     parent;
    char        name[10];
    bool        is_dir;
    int32_t     first_file_id; //special for directory
    int32_t     next_block;
    u_int32_t   num_of_blocks;
    u_int32_t   file_size;
};

struct BlockHeader
{
    int32_t     file_id;
    int32_t     next;
    u_int32_t   occ_space;
};

struct SuperBlock
{
    size_t  block_size;
    size_t  block_nums;
    int32_t root_id;
};

class MyFS
{
public:
    MyFS();
    MyFS(const char *, bool);

    int                             init();
    int                             format();
    int                             import_file(const char *, const char *);
    int                             export_file(const char *, const char *);
    int                             mkdir(const char *);
    std::string                     ls(const char *);
    int                             move(const char *, const char *);
    int                             copy(const char *, const char *);
    int                             rm(const char *);

private:
    std::string                     root_path;
    SuperBlock                      super_block;
    size_t                          total_size;
    std::vector<bool>               blocks_map;
    std::vector<u_int32_t>          descriptor_table;
    void                            create_blocks_in_root();
    void                            write_super_block();
    void                            write_free_blocks_map();
    void                            write_descriptor_table();
    std::ofstream &                 write_data_in_block(std::ofstream &, void *, int, size_t, size_t);
    SuperBlock                      read_super_block();
    std::vector<u_int32_t>          read_descriptor_table();
    std::vector<bool>               read_blocks_map();
    std::string                     get_block_name_by_id(int);
    void                            count_tables_sizes();
    void                            rewrite_file_info(MyFile);
    void                            write_new_file(MyFile, std::ifstream &);
    void                            write_new_dir(MyFile);
    void                            delete_file(MyFile &);
    void                            free_resources(MyFile &);
    size_t                          count_free_blocks_num();
    size_t                          first_free_block();
    int32_t                         first_free_fd();
    const char *                    file_preparation(const char *, MyFile &);
    MyFile                          read_file_info_by_id(size_t);
    BlockHeader                     read_block_by_id(size_t id);
    int32_t                         get_fd_by_name(MyFile &, const char *);
    int32_t                         write_file_into_fs(std::ifstream &, MyFile &, const char *, size_t, size_t, bool);
    int32_t                         write_file_from_fs(std::ofstream &, int32_t);
    size_t                          memory_to_write(int32_t);
    size_t                          blockn_for_descriptor_table;
    size_t                          max_possible_files;
    size_t                          free_blocks_num;
    void                            rm(MyFile &);
    int                             copy(MyFile &, MyFile &, const char *);
    int                             copy_file_to_dir(MyFile &, MyFile&, const char *);
};

size_t find_next_slash(const char *, size_t);

#endif // MYFS_H
