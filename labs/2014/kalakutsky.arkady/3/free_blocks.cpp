#include <iostream>

#include "fsinfo.h"
using std::cout;
using std::cin;
using std::endl;

int main(int argc, char** argv){
	if (argc != 2){
		std::cerr << "Usage: freespace <root>" << endl;
		return 1;
	}
	FSInfo fsinfo(argv[1]);
	cout << fsinfo.free_space() << endl;
	return 0;
}
