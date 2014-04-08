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
		std::cerr << "Usage: move <root> <src> <dst>" << endl;
		return 1;
	}
	try{
		FSInfo fsinfo(argv[1]);
		File root(fsinfo);

		vector <File*> src_path;
		src_path.push_back(&root);
		open_path(src_path, fsinfo, argv[2]);
		if (!src_path.back()->exists()){
			throw FileNotFoundException();
		}
		
		vector <File*> dst_path;
		dst_path.push_back(&root);
		open_path(dst_path, fsinfo, argv[3]);
		if (dst_path.back()->exists()){
			throw AlreadyExistsException();
		}
		
		src_path.back()->move_to(dst_path.back());
		
		while (dst_path.size() > 1){
			dst_path.back()->write();
			dst_path.pop_back();
		}
		while (src_path.size() > 1){
			src_path.back()->write();
			src_path.pop_back();
		}
		root.write();
		fsinfo.close();
		
					
	}catch(FSException &e){
		std::cerr << e.what() << endl;
		return 1;
	}	
	
	return 0;
}
