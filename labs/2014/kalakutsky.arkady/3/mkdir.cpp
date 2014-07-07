#include <iostream>
#include <fstream>
#include <vector>
#include "except.h"
#include "util.h"
#include "fsinfo.h"
#include "file.h"

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
	if (argc != 3){
		std::cerr << "Usage: mkdir <root> <path>" << endl;
		return 1;
	}
	try{
		FSInfo fsinfo(argv[1]);
		File root(fsinfo);

		vector <File*> path;
		path.push_back(&root);
		open_path(path, fsinfo, argv[2]);
		if (path.back()->exists()){
			throw AlreadyExistsException();
		}
		path.back()->file_data = new char[fsinfo.block_size];
		for (size_t i = 0; i != fsinfo.block_size; i ++){
			path.back()->file_data[i] = 0;
		}
		path.back()->size(fsinfo.block_size);
		path.back()->create(filetype::DIR);
		
		while (path.size() > 1){
			path.back()->write();
			path.pop_back();
		}
		root.write();
		fsinfo.close();
		
					
	}catch(FSException &e){
		std::cerr << e.what() << endl;
		return 1;
	}	
	return 0;
}
