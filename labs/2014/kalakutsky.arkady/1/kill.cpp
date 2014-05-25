#include "command_handler.h"
#include <iostream>
#include <signal.h>
#include <cstdio>

using std::cout;
using std::endl;

void kill(vector<string> const & command_tokens){
	int sig = 0;
	int pid = 0;
	if (command_tokens.size() != 3 || 
		sscanf(command_tokens[1].c_str(), "%d", &sig) + sscanf(command_tokens[2].c_str(), "%d", &pid) != 2){
		cout << "usage: kill signal pid" << endl;
		
	}else{
		kill(pid, sig);
	}
	
}
