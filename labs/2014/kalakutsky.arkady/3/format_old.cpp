#include <iostream>
#include <fstream>
#include "util.h"

using std::cout;
using std::cin;
using std::endl;

using std::ifstream;
using std::ofstream;
using std::fstream;

using std::string;
using std::to_string;


int main(int argc, char** argv){
	if (argc != 2){
		std::cerr << "Usage: format <path>" << endl;
		return 1;
	}
	
	
	string root_path = argv[1];
	size_t block_size = 0;
	size_t block_count = 0;
	fstream block0((root_path+"/0").c_str(), std::ios::binary|std::ios::out|std::ios::in);
	assertFile(block0);
	b_read(block0, block_size);
	b_read(block0, block_count);
	
	
	size_t busy_list_size = calc_busy_list_size(block_size, block_count);
	size_t blist_size_blocks = (busy_list_size + busy_list_offset()) / block_size + 1;
	size_t fst_free = blist_size_blocks + 1;

	dir_entry root("root", filetype::DIR);
	root.fst_block = root.lst_block = blist_size_blocks;
	root.size = block_size;

	block0.seekp(sizeof(block_size)+sizeof(block_count), std::ios_base::beg);
	b_write(block0, fst_free);
	b_write(block0, root);
	for (Link i = 1 ; i < blist_size_blocks-1; ++i){
		b_write(block0, i);
	}
	Link end = 0;
	b_write(block0, end); //end of list representing list itself 
	b_write(block0, end); //begin and end of root dir

	//list of free blocks
	Link written_links = fst_free + 1;
	size_t list_len = block_count;
	
	for(; written_links < (block_size - busy_list_offset())/sizeof(Link) && 
			written_links < list_len - 1; ++written_links){
		b_write(block0, written_links);
	}
	//if everything fits in first block just put end of list
	if (written_links < (block_size - busy_list_offset())/sizeof(Link)){
		block0.write("", 1);
	} else { // else write other blocks of busy list
		size_t links_in_fst_block = written_links;
		for(size_t written_lblocks = 1; written_lblocks < blist_size_blocks; written_lblocks ++){
			fstream block((root_path + "/" + to_string(written_lblocks)).c_str(),
						  std::ios::binary | std::ios::out |std::ios::in);
			assertFile(block);
			for (;written_links != links_in_fst_block+block_size/sizeof(Link) * written_lblocks  && 
					 written_links < list_len - 1; written_links ++){
				b_write(block, written_links);
			}
			//do not forget about end of busy list
			if (written_links < (block_size)/sizeof(Link) * written_lblocks){
				block.write("", 1);
			}
		}

	}
	//prepare root
	{
		fstream root_block((root_path + "/" + to_string(root.fst_block)).c_str(),
							std::ios::binary | std::ios::out | std::ios::in);
		assertFile(root_block);
		dir_entry void_entry;
		for (size_t wr_size = 0; wr_size+ sizeof(void_entry) < block_size; wr_size += sizeof(void_entry)){
			b_write(root_block, void_entry);
		}
	}
	return 0;
}















