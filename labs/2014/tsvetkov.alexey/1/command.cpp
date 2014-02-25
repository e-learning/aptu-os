#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip> 
#include <iterator>
#include <string>
#include <sstream>
#include <vector>

#include <cctype>
#include <cerrno>
#include <climits>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <unistd.h>

#include "command.hpp"

static void print_error() {
  std::cerr << "Error: " << strerror(errno)
            << std::endl;
}

static void print_error(std::string reason) {
  std::cerr << "Error: " << reason
            << std::endl;
}

static void list_dir(std::string path, 
              std::vector<std::string>& entries) {
  DIR* dir = opendir(path.c_str()); 
  dirent* entry;

  if (dir == NULL) {
    print_error();
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    entries.push_back(entry->d_name);
  }

  closedir(dir);
}

void LsCommand::run() {
  std::vector<std::string> entries;
  list_dir(".", entries);

  for (std::string entry : entries) {
    std::cout << entry << std::endl;
  }
}

void PwdCommand::run() {
  char buf[PATH_MAX];

  if (getcwd(buf, PATH_MAX)) {
    std::cout << buf << std::endl;
  } else {
    print_error();
  }
}

void PsCommand::run() {
  print_row("PID", "NAME");

  std::vector<std::string> processes;
  list_processes(processes);

  for (std::string pid : processes) {
    std::string line;
    std::string col;
    std::string name;
    std::ifstream ifs;
    std::istringstream iss;
  
    ifs.open(PROC_DIR + pid + "/status");

    if(!ifs.good()) {
      continue;
    }

    do {
      std::getline(ifs, line);
      iss.str(line);

      iss >> col;
    } while (iss.good() && col != "Name:");
    
    if(col == "Name:") {
      iss >> name;
      print_row(pid, name);
      col.clear();
    }
  }
}

void PsCommand::list_processes(std::vector<std::string>& processes) {
  using StringPred = std::function<bool(std::string)>;

  std::vector<std::string> entries;
  StringPred is_number = [] (std::string s) { 
    return std::all_of(s.begin(), s.end(), isdigit); 
  };

  list_dir(PROC_DIR, entries);
  std::copy_if(entries.begin(), entries.end(),
               std::back_inserter(processes), is_number);
}

void PsCommand::print_row(std::string pid, std::string name) {
  std::cout<< std::setw(8) << pid 
           << std::setw(20) << name << std::endl;
}

void KillCommand::run() {
  std::vector<int> signals;
  std::vector<int> pids;

  for (std::string arg : args_) {
    size_t offset = 0;
    if (!arg.empty() && arg[0] == '-') {
      offset = 1;
    }

    for (size_t i = offset; i < arg.size(); ++i) {
      if (arg[i] < '0' || arg[i] > '9') {
        print_error("Error: kill argument is not a number");
        return;
      }
    }

    if (!arg.empty() && arg[0] == '-') {
      signals.push_back(-std::stoi(arg));
    } else {
      pids.push_back(std::stoi(arg));
    }
  }

  if (signals.size() > 1) {
    print_error("Warning: more than one signal specified (sending only first)");
    return;
  }

  if (pids.empty()) {
    print_error("Error: at least one pid is mandatory");
    return;
  }

  for (int pid : pids) {
    int res;

    if (!signals.empty()) {
      res = kill(pid, signals[0]);
    } else {
      res = kill(pid, SIGTERM);
    } 

    if (res < 0) {
      print_error();
      return;
    }
  }
}

void ExitCommand::run() {
  std::exit(EXIT_SUCCESS);
}

void RunCommand::run() {
  int res = system(command_.c_str());
  if (res < 0) {
    print_error();
  }
}