#include <dirent.h> 
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

bool isSpace(char c) {
	return c == ' ' || c == '\t';
}

bool isNumeric(char c) {
	return c >= '0' && c <= '9';
}

bool isNumeric(string const & s) {
	for(size_t i = 0; i < s.length(); ++i) {
		if(!isNumeric(s[i])) return 0;
	}
	return s.length();
}

vector<string> split(string const & s) {
	vector<string> ans;
	string buffer;
	for(size_t i = 0; i < s.length(); ++i) {
		if(isSpace(s[i])) {
			if(buffer != "") {
				ans.push_back(buffer);
				buffer = "";
			}
		} else {
			buffer += s[i];
		}
	}
	if(buffer != "") {
		ans.push_back(buffer);
	}
	return ans;
}


void listProcess(string const & process_id)
{ 
	string path = "/proc/" + process_id + "/comm";
	std::ifstream procfile(path.c_str(), std::ifstream::in);
	string name;
	procfile >> name;
	procfile.close();
	cout << std::setw(5) << process_id << " " << name << endl;
}

void doPs() {
	cout << "  PID" << " CMD" << std::endl;
	dirent* dir;
	const char* proc_path = "/proc";
	DIR* proc_dir = opendir(proc_path);
	if (proc_dir)
	{
		while ((dir = readdir(proc_dir)) != NULL)
		{
			if (isNumeric(dir->d_name))
			listProcess(dir->d_name);
		}
		closedir(proc_dir);
	}
}

void doLs() {
	DIR* d;
	dirent* dir;
	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			cout << dir->d_name << endl;
		}
		closedir(d);
	}
}

void doKill(vector<string> const & command) {
	int signal = std::stoi(command[1].substr(1));
	int pid = std::stoi(command[2]);

	if(kill(pid, signal) < 0) {
		std::cout << "Error(" << errno << "): " << strerror(errno) << std::endl;
	}
}

void mainLoop() {
	string username = getlogin();
	const size_t host_length = 256;
	char hostname[host_length];
	gethostname(hostname, host_length);
	string host(hostname);

	const size_t path_size = 256;
	char path[path_size];
	if (!getcwd(path, path_size)) 
		std::cerr << "Unable to determine working dir or the workdir was too long." << endl;
	string cur_path(path);

	while (1) {
		cout << username << "@" << host << ":" << cur_path << "$ ";

		string s;
		getline(cin, s);
		vector<string> command = split(s);
		if(command.size()) {
			if(command[0] == "exit") {
				break;
			}
			if(command[0] == "ps") {
				doPs();
				continue;
			}
			if(command[0] == "pwd") {
				cout << cur_path << endl;
				continue;
			}
			if(command[0] == "ls") {
				doLs();
				continue;
			}
			if(command[0] == "kill" && command.size() == 3) {
				doKill(command);
				continue;
			}
		}
		int result = std::system(s.c_str());
		if(!result) {
			//...
		}
	}
}

int main ()
{
	mainLoop();
	return 0;
}