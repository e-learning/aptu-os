#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <cstdlib>
#include <cctype>
#include <cstdio>

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>


struct Command
{
    std::string raw;
    std::string command;
    std::vector<std::string> args;
};


Command parse(std::string const &str)
{
    Command result;
    result.raw = str;

    std::istringstream stream(str);
    stream >> result.command;

    std::string arg;
    while (stream >> arg) {
        result.args.push_back(arg);
    }

    return result;
}


bool checkEmptyArgs(Command const &cmd)
{
    if (cmd.args.empty())
        return true;
    std::cerr << "'" << cmd.command << "' must have no arguments" << std::endl;
    return false;
}


void runPwd(Command const &cmd)
{
    if (!checkEmptyArgs(cmd))
        return;
    char *pwd = get_current_dir_name();
    std::cout << pwd << std::endl;
    free(pwd);
}


void runLs(Command const &cmd)
{
    if (!checkEmptyArgs(cmd))
        return;

    DIR *dir = opendir(".");
    if (!dir) {
        std::cerr << "Error" << std::endl;
        return;
    }

    dirent *file;
    std::vector<std::string> files;
    while ((file = readdir(dir)) != 0) {
        files.push_back(file->d_name);
    }
    std::sort(files.begin(), files.end());
    std::copy(files.begin(), files.end(),
              std::ostream_iterator<std::string>(std::cout, "\n"));

    closedir(dir);
}


void runPs(Command const &cmd)
{
    if (!checkEmptyArgs(cmd))
        return;

    std::string const proc("/proc/");
    DIR *dir = opendir(proc.c_str());
    if (!dir) {
        std::cerr << "Error" << std::endl;
        return;
    }

    dirent *file;
    while ((file = readdir(dir)) != 0) {
        std::string const pid(file->d_name);

        std::ifstream fin((proc + pid + "/comm").c_str());
        if (!fin || pid == "self") continue;
        std::string comm;
        fin >> comm;

        std::cout << pid << "\t" << comm << std::endl;
    }

    closedir(dir);
}


bool isDigit(std::string const &str)
{
    return std::find_if(str.begin(), str.end(),
                        std::not1(std::ptr_fun(isdigit))) == str.end();
}


void runKill(Command const &cmd)
{
    std::vector<std::string> const &args = cmd.args;
    if (args.size() != 2 || !isDigit(args[0]) || !isDigit(args[1])) {
        std::cerr << "'kill' must have two numeric arguments" << std::endl;
        return;
    }
    if (kill(atoi(args[1].c_str()), atoi(args[0].c_str())))
        std::cerr << "Error" << std::endl;
}


bool execute(Command const &cmd)
{
    if (cmd.command.empty())
        return true;

    if (cmd.command == "exit")
        return false;

    if (cmd.command == "pwd")
        runPwd(cmd);
    else if (cmd.command == "ls")
        runLs(cmd);
    else if (cmd.command == "ps")
        runPs(cmd);
    else if (cmd.command == "kill")
        runKill(cmd);
    else
        system(cmd.raw.c_str());

    return true;
}


int main()
{
    while (true) {
        std::cout << ">> ";

        std::string str;
        std::getline(std::cin, str);

        if (!execute(parse(str)))
            break;
    }

    return 0;
}
