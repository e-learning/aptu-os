#include "command_handler.h"
#include <iostream>
#include <dirent.h>
using std::cout;
using std::cerr;
using std::endl;

void pwd (vector<string> const & command_tokens){
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL){
		cout << cwd << endl;
	}else{
		cerr << "Error during pwd command execution." << endl;
	}
}
