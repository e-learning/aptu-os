#include <iostream>
#include <fstream>
#include <vector>
#include "util.h"

using std::cout;
using std::cin;
using std::endl;

using std::ifstream;
using std::ofstream;
using std::fstream;

using std::string;
using std::to_string;

using std::vector;

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
	size_t fst_free = blist_size_blocks + 2;

	dir_entry root("root", filetype::DIR);
	root.fst_block = root.lst_block = blist_size_blocks + 1;
	root.size = block_size;

	block0.seekp(sizeof(block_size)+sizeof(block_count), std::ios_base::beg);
	b_write(block0, fst_free);
	b_write(block0, root);
	
	//Link array
	size_t written_links = 0;
	vector<Link> links(block_count, 0);
	for(;  written_links < blist_size_blocks - 1; ++written_links){
		links[written_links] = written_links + 1; //pointer to next
	}
	//ignoring elements (leave them void)
	written_links += 1;  //last element of unused list
	written_links += 1;  //block for root directory
	written_links += 1;  //block for root directory
	for (; written_links < block_count-1; ++written_links){
		links[written_links] = written_links + 1;
	}
	size_t cur_block = 0;
	for (size_t i = 0; i < block_count; i++){
		if (block0.tellp() > block_size - sizeof(Link)){
			block0.close();
			block0.open((root_path+"/"+to_string(++cur_block)).c_str());
			assertFile(block0);
		}
		b_write(block0, links[i]);
	}

	
	//prepare root directory
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















