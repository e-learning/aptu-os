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
		std::cerr << "Usage: ls <root> <file>" << endl;
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

		cout << path.back()->name() << " " << 
			path.back()->size() << " " << path.back()->time() << endl;

		if (path.back()->type() == filetype::DIR){
			path.back()->open();
			const dir_entry* dir_data = reinterpret_cast<dir_entry*>(
				path.back()->file_data);
			for (size_t i = 0; i < path.back()->size() 
					 / sizeof(dir_entry); i+=1){
				if (dir_data[i].busy)
					cout << dir_data[i];
			}
		
		}			
	}catch(FSException &e){
		std::cerr << e.what() << endl;
		return 1;
	}
	return 0;
}
