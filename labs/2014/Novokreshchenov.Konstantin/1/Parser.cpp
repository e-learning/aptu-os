#include "Parser.h"
#include "unistd.h"
#include <stdlib.h>
#include <signal.h>
#include <algorithm>
#include <fstream>
#include <dirent.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>


static size_t BUFSIZE = 1024;


static bool is_number (std::string const & str)
{
    std::string::const_iterator it = str.begin();
    while (it != str.end() && isdigit(*it)) {
        ++it;
    }
    return !str.empty() && it == str.end();
}


static int getdir (char const * dirpath, std::vector<std::string> & files)
{
    DIR* dp;
    dirent *file;
    if ((dp = opendir(dirpath)) == NULL) {
        std::cout << "shell: " << errno << std::endl;
        return ERROR;
    }

    while ((file = readdir(dp)) != NULL) {
        files.push_back(file->d_name);
    }
    closedir(dp);
    std::sort (files.begin(), files.end());

    return 0;
}


static void trim (std::string & str)
{
    std::string::iterator it;
    for (it = str.begin(); it != str.end(); ++it) {
        if ((*it != ' ') && (*it != '\t')) {
            break;
        }
    }
    str = std::string (it, str.end());
}


int print_prompt ()
{
    char hostname [BUFSIZE];
    if (gethostname (hostname, BUFSIZE)) {
        return ERROR;
    }

    char username [BUFSIZE];
    if (getlogin_r (username, BUFSIZE)) {
        return ERROR;
    }

    char curdir [BUFSIZE];
    if (! getcwd (curdir, BUFSIZE)) {
        return ERROR;
    }

    std::cout << username << "@" << hostname << ":" << curdir << "$ ";
    return 0;

}


void parse_input (std::string & input, std::vector<std::string> & command)
{
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);

    while (! input.empty()) {
        trim (input);
        std::string::iterator it;
        for (it = input.begin(); it != input.end(); ++it) {
            if ((*it == ' ') || (*it == '\t')) {
                break;
            }
        }
        if (it != input.begin()) {
            command.push_back(std::string (input.begin(), it));
            input = std::string (it, input.end());
        }
    }
}


void create_parsers (std::vector<CommandParser*> & parsers)
{
    parsers.push_back (new LsParser);
    parsers.push_back (new PwdParser);
    parsers.push_back (new PsParser);
    parsers.push_back (new KillParser);
    parsers.push_back (new ExitParser);
}


void delete_parsers (std::vector<CommandParser*> & parsers)
{
    for (std::vector<CommandParser*>::const_iterator it = parsers.begin(); it != parsers.end(); ++it) {
        delete (*it);
    }
}


static bool comp_str (std::string const & s1, std::string const & s2)
{
    return (atoi(s1.c_str()) < atoi(s2.c_str()));
}


int LsParser::parse(const std::vector<std::string> &command)
{
    if (command[0] != "ls") {
        return NOT_MATCHED;
    }
    if (command.size() >= 2) {
        std::cout << "ls" << std::endl;
        return MATCHED;
    }

    char curdir [BUFSIZE];
    getcwd (curdir, sizeof(curdir));

    std::vector<std::string> files;
    getdir (curdir, files);
    print (files);

    return  MATCHED;
}


int PwdParser::parse(const std::vector<std::string> &command)
{
    if (command[0] != "pwd") {
        return NOT_MATCHED;
    }
    if (command.size() >= 2) {
        std::cout << "pwd" << std::endl;
        return MATCHED;
    }
    char curdir [BUFSIZE];
    getcwd (curdir, sizeof(curdir));
    std::cout << curdir << std::endl;

    return MATCHED;
}


int PsParser::parse(const std::vector<std::string> &command)
{
    if (command[0] != "ps") {
        return NOT_MATCHED;
    }

    std::vector<std::string> proc_fs;
    getdir ("/proc", proc_fs);

    //list of PID
    for (std::vector<std::string>::iterator it = proc_fs.begin(); it != proc_fs.end(); ++it) {
        if (! is_number(*it)) {
            proc_fs.erase(it, it + 1);
            --it;
        }
    }
    std::sort (proc_fs.begin(), proc_fs.end(), comp_str);
    //end of list PID

    for (std::vector<std::string>::iterator it = proc_fs.begin(); it != proc_fs.end(); ++it) {
        *it = get_process_name(*it);
    }
    sort (proc_fs.begin(), proc_fs.end());
    print (proc_fs);

    return MATCHED;
}


std::string PsParser::get_process_name(const std::string &pid)
{
    std::fstream fs;
    std::string path = "/proc/" + pid + "/status";
    fs.open (path.c_str(), std::ios_base::in);
    std::string buf = "";
    if (fs.is_open()) {
        fs >> buf;
        fs >> buf;
    }
    return buf;
}


KillParser::KillParser()
{
    signal_map["sighup"] = SIGHUP;
    signal_map["sigquit"] = SIGQUIT;
    signal_map["sigkill"] = SIGKILL;
    signal_map["sigterm"] = SIGTERM;
}


int KillParser::parse(const std::vector<std::string> & command)
{
    if (command[0] != "kill") {
        return NOT_MATCHED;
    }

    if (command.size() == 1) {
        print_command_info ();
        return MATCHED;
    }

    int sig = 15;
    std::vector<std::string>::const_iterator it = command.begin() + 1;

    if (command[1][0] == '-') {
        std::string opt = command[1].substr(1);
        if ((opt == "l") && (command.size() == 2)) {
            print_signal_list ();
            return MATCHED;
        }
        else if (is_number(opt)) {
            sig = atoi(opt.c_str());
        }
        else if (signal_map.find(opt) != signal_map.end()) {
            sig = signal_map[opt];
        }
        else {
            std::cout << "kill: " << opt << ": invalid signal specification" << std::endl;
            return MATCHED;
        }
        if (command.size() == 2) {
            print_command_info();
            return MATCHED;
        }
        ++it;
    }

    for (; it != command.end(); ++it) {
        if (is_number(*it)) {
            if (kill (atoi((*it).c_str()), sig) == -1) {
                std::cout << "kill: " << *it << ": " << strerror(errno) << std::endl;
            }
        }
        else {
            std::cout << "kill: " << *it << ": arguments must be process ID" << std::endl;
        }
    }
    return MATCHED;
}


void KillParser::print_signal_list ()
{
    for (std::map<std::string, int>::const_iterator it = signal_map.begin(); it != signal_map.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}


void KillParser::print_command_info ()
{
    std::cout << "kill: usage: kill [-s sigspec | -n signum | -sigspec] pid | jobspec ... or kill -l [sigspec]" << std::endl;
}


int ExitParser::parse (const std::vector<std::string> &command)
{
    if (command[0] != "exit") {
        return NOT_MATCHED;
    }
    if (command.size() > 1) {
        std::cout << "exit" << std::endl;
        return MATCHED;
    }
    return EXIT;
}













