#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <iomanip>
#include <pwd.h>
#include <sys/wait.h>

extern void ps();

class Command {
public:
	const std::string command;
	const std::vector<std::string> args;
	Command(const std::string &command, const std::vector<std::string> &args) : command(command), args(args) {};
};

void handle_token(std::string const &token, std::string &command, std::vector<std::string> &args) {
	if (!token.empty()) {
		if (command.empty()) {
			command = token;
		} else {
			args.push_back(token);
		}
	}
}

Command parse_command(std::string const &s) {
	std::string command;
	std::vector<std::string> args;

	std::string token;

	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it) {
		if (*it != ' ') {
			token.push_back(*it);
		} else {
			handle_token(token, command, args);
			token.clear();
		}
	}

	handle_token(token, command, args);

	return Command(command, args);
}

void sigint_handler(int signum) {
	std::cout << "BYE!" << std::endl;
	exit(signum);
}

int main(int argc, char** argv) {
	signal(SIGINT, sigint_handler);
	std::string line;

	while (true) {
		std::cout << "> ";

		std::getline(std::cin, line);

		if (std::cin.eof()) {
			break;
		}

		Command command = parse_command(line);

		if (command.command == "exit") {
			break;
		}

		if (command.command == "kill") {
			if (command.args.size() != 2) {
				std::cout << "Type SIGNAL and PID" << std::endl;
				continue;
			}

			const int signal = atoi(command.args[0].c_str());
			const int pid = atoi(command.args[1].c_str());

			if (kill(pid, signal)) {
				std::cout << "ERROR" << std::endl;
			} else {
				std::cout << "OK" << std::endl;
			}
		} else if (command.command == "pwd") {
			std::cout << get_current_dir_name() << std::endl;
		} else if (command.command == "ps") {
			ps();
		} else if (command.command == "ls") {
			const char* path;

			if (command.args.empty()) {
				path = ".";
			} else if (command.args[0] == "~") {
				path = getpwuid(getuid())->pw_dir;
			} else {
				path = command.args[0].c_str();
			}

			DIR* dir = opendir(path);

			if (dir == NULL) {
				std::cout << "ERROR" << std::endl;
				continue;
			}

			struct dirent* current;

			while ((current = readdir(dir)) != NULL) {
				const std::string name = std::string(current->d_name);

				if (name == "." || name == "..") {
					continue;
				}

				std::cout << name << std::endl;
			}

			closedir(dir);
		} else if (!command.command.empty()) {
			const pid_t pid = fork();

			if (pid < 0) {
				std::cout << "ERROR" << std::endl;
			} else if (pid == 0) {
				char **args = new char*[command.args.size() + 2];

				args[0] = new char[command.command.size()];
				strcpy(args[0], command.command.c_str());

				for (size_t i = 1; i < command.args.size() + 1; ++i) {
					args[i] = new char[command.args[i - 1].size()];
					strcpy(args[i], command.args[i - 1].c_str());
				}

				args[command.args.size() + 1] = NULL;

				execvp(command.command.c_str(), args);

				for (size_t i = 0; i < command.args.size() + 1; ++i) {
					delete[] args[i];
				}

				delete[] args;

				exit(0);
			} else {
				int status;
				waitpid(pid, &status, 0);
				std::cout << status << std::endl;
			}
		}
	}

	return 0;
}