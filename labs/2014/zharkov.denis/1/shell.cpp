#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <iterator>
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/types.h>
#include <signal.h>

std::pair< std::string, std::vector< std::string > > split_cmd(std::string cmd) {
    std::istringstream iss(cmd);
    std::vector< std::string > args{
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };

    std::string cmd_name = args[0];

    return std::make_pair(cmd_name, args);
}

void ls_cmd(std::string const & path) {
    DIR * d = opendir(path.length() > 0 ? path.c_str() : ".");

    if (d == 0) {
        std::cout << "error" << std::endl;
        return;
    }
    
    dirent * dp = 0;

    while ((dp = readdir(d)) != 0) {
        std::cout << dp->d_name << " ";
    }

    std::cout << std::endl;

    closedir(d);
}

int get_pid(char const * str) {
    for (char const * p = str; *p != 0; p++) {
        if (!isdigit(*p)) {
            return -1;
        }
    }

    std::stringstream ss(str);
    int pid;
    
    ss >> pid;

    return pid;
}

std::pair<std::string, std::string> parse_proc_option(std::string option) {
    int i = 0;
    for (; option[i] != ':'; i++);

    int j = i+1;
    for (; option[j] == ' ' || option[j] == '\t'; j++);

    return std::make_pair(option.substr(0, i), option.substr(j));
}

void ps_cmd() {
    DIR * d = opendir("/proc");

    if (d == 0) {
        std::cout<< "error" << std::endl;
        return;
    }
    
    std::vector< int > pids;

    dirent * dp = 0;
    
    while ((dp = readdir(d)) != 0) {
        int pid = -1;
        if ((pid = get_pid(dp->d_name)) != -1) {
            pids.push_back(pid);
        }
    }
    
    closedir(d);
    
    static const std::vector<std::string> columns = {"Name", "Pid", "VmSize"};
    const int column_width = 15;

    for (auto & column : columns) {
        std::cout << std::setw(column_width) << std::left << column << " ";
    }

    std::cout<< std::endl;

    for (int pid : pids) {
        std::ifstream options_file("/proc/" + std::to_string(pid) + "/status");
        
        std::map<std::string, std::string> desc;

        std::string option_string = "";
        while (std::getline(options_file, option_string)) {
            auto option = parse_proc_option(option_string);
            if (std::find(columns.begin(), columns.end(), option.first) != columns.end()) {
                desc[option.first] = option.second;
            }
        }

        for (auto & column : columns) {
            std::cout << std::setw(column_width) << std::left << desc[column] << " ";
        }

        std::cout<< std::endl;
    }

    std::cout << std::endl;
}

void pwd_cmd() {
    char path[MAXPATHLEN];
    getcwd(path, MAXPATHLEN);
    
    std::cout << path << std::endl;
}

void kill_cmd(std::string signal_string, std::string pid_string) {
   if (signal_string[0] == '-') {
       signal_string = signal_string.substr(1);
   }

   int signal = std::stoi(signal_string);
   pid_t pid = std::stoi(pid_string);

   kill(pid, signal);
}

int main() {
    while (true) {
        std::cout << "# ";
        std::cout.flush();
    
        std::string cmd = "";

        if (std::getline(std::cin, cmd)) {
            auto scmd = split_cmd(cmd);
            
            std::string cmd_name = scmd.first;
            std::vector<std::string> args = scmd.second;
    
            if (cmd_name == "ls") {
                ls_cmd(args.size() > 1 ? args[1] : "");
            } else if (cmd_name == "ps") {
                ps_cmd();
            } else if (cmd_name == "pwd") {
                pwd_cmd();
            } else if (cmd_name == "kill") {
                if (args.size() < 3) {
                    std::cout << "two arguments expected" << std::endl;
                    continue;
                }
                kill_cmd(args[1], args[2]);
            } else if (cmd_name == "exit") {
                return 0;
            } else {
                system(cmd.c_str());
            }
        }
    }

    return 0;
}
