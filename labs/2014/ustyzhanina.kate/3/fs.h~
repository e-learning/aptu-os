#ifndef FS_H
#define FS_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <stdexcept>
using namespace std;

#include "directory.h"
#include "utils.h"

class  FileSystem
{
  public:
	FileSystem(string m_dirname)
		: root_dirname(m_dirname) {}
	void init();
	void format();
	void import(string from, string to);
	void export_(string from, string to);

	void rm_file(string const from);

	void move(string  const source, string const to);
	void copy(string  source, string to);

	void mkdir(string path);
	void ls(string path);
    size_t free_blocks_count;
  private:
	Utils utils;

    const  size_t DIR = 1;
    const  size_t FILE = 2;

	string root_dirname;

	size_t all_blocks_count;
	size_t size;
	size_t meta_blocks_count;
	size_t main_info_size;
	vector <size_t> meta_info;

    void rm_one_file(DirectoryD source);
	void write_meta();

	//read config file
	void read_config();

	//blocks count
	void fix_free_blocks();

	void read_meta_data();
	void write_directory(DirectoryD dir, size_t first_block);
	DirectoryD read_directory(size_t block);
	//write out file
	size_t write_out_file_to_blocks(FileD file_m, string in_path);
	//write out file
	size_t write_file_to_blocks(FileD file_m);
	void fill_free_file(ofstream  & out, size_t need_size);
	void fill_free_block(vector <size_t> blocks);

	FileD read_file_from_blocks(size_t first_block);



	pair<string, size_t>  find_directory(string instr, bool up_to_end);
    pair<string, size_t>  find_directory_not(string instr, bool up_to_end);
	bool check_exist_file(string in_file, queue<pair<string, size_t> > cur_list);


	void modify_file(string from, string to, bool fl);


	void copy_directory(DirectoryD source, DirectoryD to);

    size_t mk_one_dir(DirectoryD  parent_dir, string name);
	size_t find_first_empty();

	size_t alloc_block();

	size_t alloc_block(size_t num);
};


#endif // FS_H
