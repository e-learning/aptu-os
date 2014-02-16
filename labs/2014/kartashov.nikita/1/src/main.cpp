#include <dirent.h> 
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>

using std::ifstream;
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;

void ls(string directory)
{
  DIR* d;
  struct dirent* dir;
  d = opendir(directory.c_str());
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      cout << dir->d_name << endl;
    }
    closedir(d);
  }
}

const string kill_command = "kill";

vector<string> split(string const& s, char delim)
{
  vector<string> result;
  string buf;
  for (size_t i = 0; i < s.size(); ++i)
  {
    if (s[i] == delim)
    {
      if (!buf.empty())
      {
        result.push_back(buf);
        buf = "";
      }
    }
    else
      buf += s[i];
  }
  if (!buf.empty())
    result.push_back(buf);
  return result;
}

int resolve_signal(string signal)
{
  if (signal == "SIGABRT")
    return SIGABRT;
  if (signal == "SIGALRM")
    return SIGALRM;
  if (signal == "SIGFPE")
    return SIGFPE;
  if (signal == "SIGHUP")
    return SIGHUP;
  if (signal == "SIGILL")
    return SIGILL;
  if (signal == "SIGINT")
    return SIGINT;
  if (signal == "SIGKILL")
    return SIGKILL;
  if (signal == "SIGPIPE")
    return SIGPIPE;
  if (signal == "SIGQUIT")
    return SIGQUIT;
  if (signal == "SIGSEGV")
    return SIGSEGV;
  if (signal == "SIGTERM")
    return SIGTERM;
  if (signal == "SIGUSR1")
    return SIGUSR1;
  if (signal == "SIGUSR2")
    return SIGUSR2;
  if (signal == "SIGCHLD")
    return SIGCHLD;
  if (signal == "SIGCONT")
    return SIGCONT;
  if (signal == "SIGSTOP")
    return SIGSTOP;
  if (signal == "SIGTSTP")
    return SIGTSTP;
  if (signal == "SIGTTIN")
    return SIGTTIN;
  if (signal == "SIGTTOU")
    return SIGTTOU;
  if (signal == "SIGBUS")
    return SIGBUS;
  if (signal == "SIGPROF")
    return SIGPROF;
  if (signal == "SIGSYS")
    return SIGSYS;
  if (signal == "SIGURG")
    return SIGURG;
  if (signal == "SIGVTALRM")
    return SIGVTALRM;
  if (signal == "SIGXCPU")
    return SIGXCPU;
  if (signal == "SIGXFSZ")
    return SIGXFSZ;

  return -1;
}

bool kill_call(string const& command)
{
  vector<string> v = split(command, '\t');
  if (v[0] != kill_command)
    return false;

  int result = kill(std::atoi(v[1].c_str()), resolve_signal(v[2]));;
  if (result != 0)
    cout << "Unable to send signal " << v[2] << " to process with pid " << std::atoi(v[1].c_str()) << endl;
  return true;
}

void call(string const& application)
{
  std::system(application.c_str());
}

void pwd(string const& path)
{
  cout << path << endl;
}

bool is_numeric(string const& s)
{
  const char* p = s.c_str();
  if (!*p)
      return false;

  while(*p)
  {
    if (!isdigit(*p++))
      return false;
  }

  return true;
}

void list_process(string const& process_id)
{ 
  //PID TTY TIME CMD
  // const char* format_string = "%5s %8s %9s %s\n";
  string full_path = "/proc/" + process_id + "/status";
  ifstream status_file(full_path.c_str(), std::ifstream::in);
  string pid;
  string name;
  
  while(true)
  {
    string buf;
    status_file >> buf; 

    if (buf == "Name:")
    {
      status_file >> name;
      continue;
    }

    if (buf == "Pid:")
    {
      status_file >> pid;
      break;
    }
  }

  cout << std::setw(5) << pid << " " << name << endl;

  status_file.close();
}

void list_processes()
{
  struct dirent* dir;
  const char* proc_path = "/proc";
  DIR* proc_dir = opendir(proc_path);
  if (proc_dir)
  {
    while ((dir = readdir(proc_dir)) != NULL)
    {
      if (is_numeric(dir->d_name))
        list_process(dir->d_name);
    }
    closedir(proc_dir);
  }
}

void ps()
{
  cout << "  PID" <</* " TTY      " << "      TIME" <<*/ " CMD" << endl;
  list_processes();
}

void display_user_prompt(string const& username, 
  string const& host, 
  string const& cur_path)
{
  cout << username << "@" 
  << host << ":" 
  << cur_path << "$ ";
}

string read_user_input(string const& username, 
  string const& host, 
  string const& cur_path)
{
  display_user_prompt(username, host, cur_path);
  string command;
  std::getline(cin, command);
  return command;
}

const string ls_command = "ls";
const string pwd_command = "pwd";
const string ps_command = "ps";
const string exit_command = "exit";

void main_loop()
{
  string username = getlogin();
  const size_t host_length = 256;
  char hostname[host_length];
  gethostname(hostname, host_length);
  string host(hostname);

  const size_t path_size = 256;
  char path[path_size];
  if (!getcwd(path, path_size)) 
    cerr << "Unable to determine working dir or the workdir was too long." << endl;
  string cur_path(path);

  while(1)
  {
    string command = read_user_input(username, host, cur_path);
    
    if (command == exit_command)
      break;

    if (command == pwd_command)
    {
      pwd(cur_path);
      continue;
    }

    if (command == ls_command)
    {
      ls(".");
      continue;
    }

    if (command == ps_command)
    {
      ps();
      continue;
    }

    if (kill_call(command))
      continue;

    call(command);
  }
}

int main()
{
  main_loop();

  return 0;
}