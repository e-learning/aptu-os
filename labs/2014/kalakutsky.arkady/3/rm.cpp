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
		std::cerr << "Usage: rm <root> <file>" << endl;
		return 1;
	}
	try{
		FSInfo fsinfo(argv[1]);
		File root(fsinfo);
		
		vector <File*> path;
		path.push_back(&root);
		open_path(path, fsinfo, argv[2]);
		if (!path.back()->exists()){
			return 0;
		}
		if (path.size() == 1){
			std::cerr << "It's dangerous to operate recursively on `/'" << endl;
		}
		path.back()->del();
		
		
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
