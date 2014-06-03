#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include <signal.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "string_operation.h"

using namespace std;

typedef vector<string>::iterator s_iterator;

char buf[FILENAME_MAX];

char *pwd() {
    if (getcwd(buf, sizeof(buf))) {
        return buf;
    } else {
        return NULL;
    }
}

int main() {

    string welcome_text = ">@: ";
    string input;

    for (;;) {
        cout << welcome_text;
        getline(cin, input);
        vector<string> args = split(input);

        if (args.size() == 0) {
            goto continue_loop;
        }

        if (args[0] == "ls") {
            const char *d_path = pwd();
            if (args.size() > 1) {
                d_path = args[1].c_str();
            }
            DIR *dir;
            struct dirent *entry;
            if ((dir = opendir(d_path)) != NULL) {
                while ((entry = readdir(dir)) != NULL) {
                    cout << entry->d_name << endl;
                }
                closedir (dir);
            } else {
                cout << "ls: cannot list files in " << d_path << endl;
            }
        } else if (args[0] == "pwd") {
            char *current_dir = pwd();
            if (current_dir) {
                cout << current_dir << endl;
            } else {
                cout << "pwd: cannot locate myself" << endl;
            }
        } else if (args[0] == "ps") {
            DIR *proc_dir = opendir("/proc");
            if (!proc_dir) {
                cout << "ps: cannot access /proc" << endl;
                goto continue_loop;
            }
            struct dirent *entry;
            while ((entry = readdir(proc_dir)) != NULL) {
                if (entry->d_type == DT_DIR) {
                    string cmd = "/proc/";
                    cmd += entry->d_name;
                    cmd += "/comm";
                    cout << entry->d_name << "\t";
                    FILE* f_cmd = fopen(cmd.c_str(), "r");
                    if (!f_cmd) {
                        continue;
                    }
                    char buffer[128];
                    while(!feof(f_cmd)) {
                        if(fgets(buffer, 128, f_cmd) != NULL) {
                            cout << buffer;
                        }
                    }
                    fclose(f_cmd);
                }
            }
            closedir(proc_dir);
        } else if (args[0] == "kill") {
            if (args.size() > 1) {
                int _signal = SIGTERM;
                s_iterator pids_start = args.begin() + 1;
                if (args[1][0] == '-') {
                    string & signal_string = args[1];
                    signal_string.erase(signal_string.begin(), signal_string.begin() + 1);
                    istringstream signal_stream (signal_string);
                    signal_stream >> _signal;
                    if (signal_stream.fail()) {
                        cout << "kill: " << signal_string << ": invalid signal specification" << endl;
                        continue;
                    }
                    pids_start = args.begin() + 2;
                }
                for (s_iterator it = pids_start; it != args.end(); it++) {
                    int pid;
                    istringstream pid_stream(*it);
                    pid_stream >> pid;
                    if (pid_stream.fail()) {
                        cout << "kill: " << *it << ": arguments must be process or job IDs" << endl;
                    } else {
                        if (kill(pid, _signal)  < 0) {
                            if (errno == EINVAL) {
                                cout <<"kill: " << pid << ": invalid signal specification" << endl;
                                goto continue_loop;
                            } else if (errno == EPERM){
                                cout << "kill: (" << pid << ") - Operation not permitted" << endl;
                            } else if (errno == ESRCH){
                                cout << "kill: (" << pid << ") - No such process" << endl;
                            }
                        }
                    }
                }
            } else {
                cout << "kill: usage: kill [-sigspec] pid [pid2]..[pidn]" << endl;
            }
        } else if (args[0] == "args") {
            for (vector<string>::iterator it = args.begin(); it != args.end(); it++){
                cout << *it << endl;
            }
        } else if (args[0] == "exit") {
            break;
        } else {
            FILE* pipe = popen(input.c_str(), "r");
            if (!pipe) {
                goto continue_loop;
            }
            char buffer[128];
            while(!feof(pipe)) {
                if(fgets(buffer, 128, pipe) != NULL) {
                    cout << buffer;
                }
            }
            pclose(pipe);
        }
        continue_loop:
        ;
    }


    return 0;
}
