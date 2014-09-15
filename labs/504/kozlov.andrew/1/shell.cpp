#include <iostream>
#include <unistd.h>
#include <sys/param.h>
#include <array>
#include <vector>
#include <sstream>
#include <dirent.h>

using namespace std;

typedef const vector<string> arguments;

void print_with_prefix_and_suffix(const string& str, const string& message) {
    cout << str << ": ";

    if (!message.empty()) {
        cout << message;
    }

    cout << endl;
}

void print_dir(const string& path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        print_with_prefix_and_suffix(path, strerror(errno));
        return;
    }

    dirent* dir_entry = readdir(dir);
    while (dir_entry) {
        cout << dir_entry->d_name << endl;
        dir_entry = readdir(dir);
    }

    closedir(dir);
}

void ls_handler(arguments args) {
    if (args.size() == 0) {
        print_dir(".");
    } else {
        bool is_list = args.size() > 1;
        for (auto it = args.begin(); it != args.end(); it++) {
            if (is_list) {
                print_with_prefix_and_suffix(*it, "");
            }

            print_dir(*it);

            if (is_list && (it + 1 != args.end())) {
                cout << endl;
            }
        }
    }
}

void pwd_handler(arguments) {
    char result[MAXPATHLEN];
    getcwd(result, MAXPATHLEN);

    cout << result << endl;
}

void ps_handler(arguments) {
    DIR *pd = opendir("/proc");
    if (!pd) {
        print_with_prefix_and_suffix("ps", strerror(errno));
        return;
    }
}

void kill_handler(arguments args) {

}

void exit_handler(arguments) {
    exit(EXIT_SUCCESS);
}

typedef void (*handler)(arguments);

const array<pair<string, handler>, 5> utils = {{
        make_pair("ls", ls_handler),
        make_pair("pwd", pwd_handler),
        make_pair("ps", ps_handler),
        make_pair("kill", kill_handler),
        make_pair("exit", exit_handler)
}};

typedef pair<string, const vector<string>> command;

command split_string(const string& raw_command) {
    stringstream ss(raw_command);

    string cmd;
    ss >> cmd;

    string arg;
    vector<string> args;
    while (ss >> arg) {
        args.push_back(arg);
    }

    return make_pair(cmd, args);
}

void print_shell_prefix() {
    cout << "> ";
    cout.flush();
}

void print_command_not_found_error(const string& command) {
    cout << command << ": command not found" << endl;
}

int main() {
    while (true) {
        print_shell_prefix();

        string s = "";
        if (getline(cin, s)) {
            auto c = split_string(s);

            bool command_is_found = false;
            for (auto it = utils.begin(); it != utils.end(); ++it) {
                if (c.first == it->first) {
                    it->second(c.second);
                    command_is_found = true;
                }
            }

            if (!command_is_found) {
                print_command_not_found_error(c.first);
            }
        }
    }

    return 0;
}