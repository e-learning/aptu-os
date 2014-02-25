#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sys/param.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>

bool is_num(const char *str) {
	while (*str) {
		if (*str < '0' || *str > '9') {
			return 0;
		}
		str++;
	}

	return 1;
}

void trim(std::string &s) {
	while (isspace(*s.begin())) {
		s.erase(s.begin());
	}
	while (s.size() > 0 && isspace(*s.rbegin())) {
		s.erase(s.end() - 1);
	}
}

std::string get_current_directory() {
	char path[MAXPATHLEN];
	getcwd(path, MAXPATHLEN);
	return std::string(path);
}

struct command {
	command(std::string cmd = "") : cmd(cmd) { }
	std::string cmd;
	std::vector<std::string> args;
};

command parse_input() {
	std::string input;

	do {
		getline(std::cin, input);
		trim(input);
	} while (input.size() == 0);

	std::stringstream ss(input);
	command c;

	ss >> c.cmd;
	std::string arg;
	while (!ss.eof()) {
		ss >> arg;
		c.args.push_back(arg);
	}
	return c;
}

int exec(command const &cmd) {
	if (cmd.cmd == "ls") {
		DIR *directory;
		std::string current_dir = get_current_directory();

		if ((directory = opendir(current_dir.c_str())) == 0)
			std::cout << "reading current directory failed: " << current_dir << std::endl;
		else {
			dirent *file;
			while ((file = readdir(directory)) != 0) {
				if (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
					std::cout << file->d_name << std::endl;
			}
			closedir(directory);
		}

		return 0;
	}

	if (cmd.cmd == "pwd") {
		std::cout << get_current_directory() << std::endl;
		return 0;
	}

	if (cmd.cmd == "ps") {
		DIR *procdir;

		std::string pathprocdir = "/proc/";

		if ((procdir = opendir(pathprocdir.c_str())) == 0)
			std::cout << "reading /proc/ failed" << std::endl;
		else {
			dirent *pid;
			std::string commandLine;
			while ((pid = readdir(procdir)) != 0) {
				if (pid->d_type == DT_DIR && is_num(pid->d_name)) {
					std::string cmdFilePath = pathprocdir + std::string(pid->d_name) + "/comm";
					std::ifstream cmdFile(cmdFilePath.c_str());

					if (cmdFile.is_open()) {
						getline(cmdFile, commandLine);
						std::cout << pid->d_name << ": " << commandLine << std::endl;
					}
					cmdFile.close();
				}
			}
			closedir(procdir);
		}
		return 0;
	}

	if (cmd.cmd == "kill") {
		if (cmd.args.size() != 2 || !is_num(cmd.args[0].c_str()) ||
				!is_num(cmd.args[1].c_str())) {
			std::cout << "invalid command parameters" << std::endl;
		} else {
			int signal = atoi(cmd.args[0].c_str());
			int pid = atoi(cmd.args[1].c_str());
			kill(pid, signal);
		}
		return 0;
	}

	if (cmd.cmd == "exit") {
		return 1;
	}

	std::stringstream ss;
	ss << cmd.cmd;

	for (size_t i = 0; i < cmd.args.size(); ++i)
		ss << " " << cmd.args[i];

	system(ss.str().c_str());
	return 0;
}

int main() {
	do {
	} while (exec(parse_input()) == 0);

	return 0;
}