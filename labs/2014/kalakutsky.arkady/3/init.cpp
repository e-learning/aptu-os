#include <iostream>
#include <string>
#include <fstream>
#include "util.h"

using std::cout;
using std::cin;
using std::endl;

using std::ifstream;
using std::ofstream;

using std::string;
using std::to_string;

int main(int argc, char** argv){
	if (argc != 2){
		std::cerr << "Usage: init <path>" << endl;
		return 1;
	}
	string root_path = argv[1];
	
	ifstream config((root_path+"/config").c_str());
	assertFile(config);

	size_t block_size = 0;
	size_t block_count = 0;
	config >> block_size;
	block_size = block_size < 1024 ? 1024: block_size;
	config >> block_count;

	for (size_t b = 0; b != block_count ; ++ b){
		ofstream block((root_path+"/"+to_string(b)).c_str(), 
						  std::ios::binary | std::ios::out);
		assertFile(block);
		block.exceptions(std::fstream::badbit);
		try{
			b_write(block, block_size);
			b_write(block, block_count);
			block.seekp((block_size) - 1);
			block.write("", 1);
		}catch(...){
			std::cerr << "Can't create block" << endl;
			return 1;
		}
		
	}
	return 0;
}
