#include <iostream>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <iomanip>

bool is_digits(const std::string & s) {
    return s.find_first_not_of("0123456789") == std::string::npos;
}

void ps() {
	DIR* proc = opendir("/proc");

	if (proc == NULL) {
		std::cout << "ERROR" << std::endl;
		return;
	}

	std::cout << std::setw(6) << std::left << "PID" << " " << "CMD" << std::endl;

	struct dirent* current;

	while ((current = readdir(proc)) != NULL) {
		std::string pid = std::string(current->d_name);

		if (is_digits(pid)) {
			std::string comm_file = "/proc/" + pid + "/task/" + pid + "/comm";

			std::ifstream comm(comm_file.c_str());

			std::string cmd;
			comm >> cmd;

			std::cout << std::setw(6) << std::left << pid << " " << cmd << std::endl;
		}
	}

	closedir(proc);
}