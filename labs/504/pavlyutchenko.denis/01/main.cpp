#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <signal.h>    // kill
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <iomanip>
#include <fnmatch.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

string exitString = "exit";

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

static int countArgs = 0;

void ps();

void ls(char **args);

char ** splitInput(const std::string &s) {
    std::vector<std::string> strings = split(s, ' ');

    char** result = new char*[strings.size() + 1];

    for (unsigned int i = 0; i < strings.size(); i++) {
        result[i] = new char[strings[i].length() + 1];
        strcpy(result[i], strings[i].c_str());
    }

    result[strings.size()] = NULL;

    countArgs = strings.size();

    return result;
}

int filter(const struct dirent *dir)
{
    return !fnmatch("[1-9]*", dir->d_name, 0);
}

int main()
{
    string input = "";

    while (true) {
        cout << "> ";

        std::getline(std::cin, input);

        if (input == exitString) {
            break;
        }

        char** args = splitInput(input);

        if (args[0] == '\0') {
            continue;
        }
        else if (string(args[0]) == "kill") {
            if (countArgs != 3) {
                cout << "You must write PID and SIGNAL" << endl;
                continue;
            }

            if (kill(atoi(args[1]), atoi(args[2]))) {
                cout << " failure" << endl;
            }
            else {
                cout << "success" << endl;
            }
        }
        else if (string(args[0]) == "pwd") {
            cout << get_current_dir_name() << endl;
        }
        else if (string(args[0]) == "ps") {
            ps();
        }
        else if (string(args[0]) == "ls") {
            ls(args);
        }
        else {
            string programName = "/bin/" + string(args[0]);
            const char *cstr = programName.c_str();

            pid_t pid = fork();

            if (pid == 0) { /* child process */
                execv(cstr, args);
                exit(127); /* only if execv fails */
            }
            else { /* pid!=0; parent process */
                waitpid(pid, 0, 0); /* wait for child to exit */
            }
        }
    }

    return 0;
}

void ls(char **args) {
    char *userDir;

    if (countArgs == 2) {
        userDir = args[1];
    } else {
        userDir = new char[2];
        userDir[0] = '.';
    }

    if (string(userDir) == "~") {
        struct passwd *pw = getpwuid(getuid());

        userDir = pw->pw_dir;
    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(userDir)) != NULL) {
        vector<string> files;
        int max = 0;

        int counter = 0;

        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                if (counter % 2 == 0) {
                    int len = string(ent->d_name).length();

                    if (len > max) {
                        max = len;
                    }
                }

                counter++;
                files.push_back(string(ent->d_name));
            }
        }

        if (files.size() > 0) {
            unsigned int size = files.size();

            if (size % 2 == 1) size -= 1;

            for (unsigned int i = 0; i < size; i += 2) {
                cout << left << setw(max + 4) << files[i] << "  " << files[i + 1] << endl;
            }

            if (size != files.size()) {
                cout << files[size] << endl;
            }
        }
        closedir(dir);
    } else {
        cout << "There is no directory or you have no required rights." << endl;
    }
}

void ps() {
    ///proc/1/cmdline

    struct dirent **namelist;
    int n;

    n = scandir("/proc", &namelist, filter, 0);
    if (n < 0)
        perror("Not enough memory.");
    else {
        cout << setw(5) << "  PID   CMD" << endl;

        for (int d = 0; d < n; d++) {
            string fileName = "/proc/" + string(namelist[d]->d_name) + "/task/" + string(namelist[d]->d_name) + "/comm";
            ifstream in(fileName.c_str());
            string cmd;
            in >> cmd;

            cout << setw(5) << string(namelist[d]->d_name) << "   " << cmd << endl;

            free(namelist[d]);
        }
        free(namelist);
    }
}

