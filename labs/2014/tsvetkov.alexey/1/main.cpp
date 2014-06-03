#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "command.hpp"

Command* parse_command(std::string s);

int main(int argc, char* argv[]) {
  while (std::cin.good()) {
    std::cout << "~ " << std::flush;
    
    std::string line;
    std::getline(std::cin, line);
    
    Command* command = parse_command(line);
    command->run();
    
    delete command;
  }

  return 0;
}

Command* parse_command(std::string s) {
  std::string name;
  std::vector<std::string> args;
  std::istringstream iss(s);
  
  iss >> name;
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(args));

  if(name == "ls") {
    return new LsCommand(name, args);
  }

  if(name == "pwd") {
    return new PwdCommand(name, args);
  }

  if(name == "ps") {
    return new PsCommand(name, args);
  }

  if(name == "kill") {
    return new KillCommand(name, args);
  }

  if(name == "exit") {
    return new ExitCommand(name, args);
  }

  return new RunCommand(s, name, args);
}