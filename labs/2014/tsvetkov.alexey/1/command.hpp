#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

class Command {
public:
  Command(std::string name, std::vector<std::string> args)
  : name_(name),
    args_(args) {}

  virtual ~Command() {}
  
  virtual void run() = 0;
protected:
  std::string name_;
  std::vector<std::string> args_;
};

class LsCommand: public Command {
public:
  LsCommand(std::string name, std::vector<std::string> args)
  : Command(name, args) {}

  virtual void run();
};

class PwdCommand: public Command {
public:
  PwdCommand(std::string name, std::vector<std::string> args)
  : Command(name, args) {}

  virtual void run();
};

class PsCommand: public Command {
public:
  PsCommand(std::string name, std::vector<std::string> args)
  : Command(name, args), 
    PROC_DIR("/proc/") {}

  virtual void run();

private:
  const std::string PROC_DIR;

  void list_processes(std::vector<std::string>& processes);
  void print_row(std::string pid, std::string name);
};

class KillCommand: public Command {
public:
  KillCommand(std::string name, std::vector<std::string> args)
  : Command(name, args) {}

  virtual void run();
};

class ExitCommand: public Command {
public:
  ExitCommand(std::string name, std::vector<std::string> args)
  : Command(name, args) {}

  virtual void run();
};

class RunCommand: public Command {
public:
  RunCommand(std::string command,
             std::string name, 
             std::vector<std::string> args)
  : Command(name, args), 
    command_(command) {}

  virtual void run();
private:
  std::string command_;
};

#endif