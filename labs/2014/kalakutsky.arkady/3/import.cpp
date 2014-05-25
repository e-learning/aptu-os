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
	if (argc != 4){
		std::cerr << "Usage: import <root> <host_file> <fs_file>" << endl;
		return 1;
	}
	try{
		ifstream impfile(argv[2]);
		if (!impfile){
			std::cerr << "Can't open file " << argv[2] << endl;
			return 1;
		}

		impfile.seekg(0, impfile.end);
		size_t file_size = impfile.tellg();
		impfile.seekg(0, impfile.beg);

		FSInfo fsinfo(argv[1]);
		File root(fsinfo);

		vector <File*> path;
		path.push_back(&root);
		open_path(path, fsinfo, argv[3]);
		if (path.back()->exists()){
			throw AlreadyExistsException();
		}
		
		size_t file_data_size = (file_size /fsinfo.block_size + 1) 
			* fsinfo.block_size;
		path.back()->file_data = new char[file_data_size];
		impfile.read(path.back()->file_data, file_size);

		path.back()->size(file_size);
		path.back()->create(filetype::BIN);
		
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

