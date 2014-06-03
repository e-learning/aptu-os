#include <iostream>
#include <cstdlib>
#include <map>
#include <unistd.h>
#include <cstdio>
#include <errno.h>
#include <cstring>
#include "command_handler.h"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

using std::map;



void tokenizeString(string const & src, vector<string> & dst){
	char * src_data = new char[src.size()+1];
	strcpy(src_data, src.c_str());
	char * pch = strtok(src_data," \t");
	while (pch != NULL){
		dst.push_back(string(pch));
		pch = strtok (NULL, " \t");
	}
	delete src_data;
}

int main(){
	map<string, CommandFunc> commands;
	commands["ls"] = ls;
	commands["ps"] = ps;
	commands["pwd"] = pwd;
	commands["kill"] = kill;
	
	while(1){
		cout << '$';
		string command;
		getline(cin, command);
		vector<string> command_tokens;
		tokenizeString(command, command_tokens);
		if (command_tokens.empty()){
			continue;
		}
		if (command_tokens[0] == "exit")
			break;
		else{
			if (commands.find(command_tokens[0]) != commands.end()){
				commands[command_tokens[0]](command_tokens);
			}else{
				system(command.c_str());
			}
		}
	}


	return 0;
}
