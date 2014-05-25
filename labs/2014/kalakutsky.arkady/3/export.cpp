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
		std::cerr << "Usage: export <root> <fs_file> <host_file>" << endl;
		return 1;
	}
	try{

		FSInfo fsinfo(argv[1]);
		File root(fsinfo);

		vector <File*> path;
		path.push_back(&root);
		open_path(path, fsinfo, argv[2]);
		if (!path.back()->exists()){
			throw FileNotFoundException();
		}
		if (path.back()->type() != filetype::BIN){
			throw NotDirectoryException();
		}
		path.back()->open();
		ofstream expfile(argv[3]);
		if( ! expfile ){
			std::cerr << "Can't open file " << argv[3] << endl;
			return 1;
		}
		

		expfile.write(path.back()->file_data, path.back()->size());
		
		while (path.size() > 1){
			path.pop_back();
		}
		
					
	}catch(FSException &e){
		std::cerr << e.what() << endl;
		return 1;
	}	
	return 0;
}

