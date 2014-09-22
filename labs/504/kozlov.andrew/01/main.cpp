#include "common.h"

#include <array>
#include <sstream>
#include <stdlib.h>
#include <sys/wait.h>

extern void ls_handler(arguments);
extern void pwd_handler(arguments);
extern void ps_handler(arguments);
extern void kill_handler(arguments);
extern void exit_handler(arguments);

typedef void (*handler)(arguments);

const array<pair<string, handler>, 5> utils = {{
        make_pair("ls", ls_handler),
        make_pair("pwd", pwd_handler),
        make_pair("ps", ps_handler),
        make_pair("kill", kill_handler),
        make_pair("exit", exit_handler)
}};

typedef pair<const string, const vector<string>> command;

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

void sigint_handler(int signal_number) {
    cout << "SIGINT" << endl;
    exit(EXIT_SUCCESS);
}

char* copy_c_str(const string& s) {
    char* result = new char[s.length() + 1];
    strcpy(result, s.c_str());

    return result;
}

void exec_handler(command& cmd) {
    pid_t pid = fork();

    if (pid < 0) {
        print_with_prefix_and_suffix(cmd.first, "fork() error");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        arguments args = cmd.second;
        char** args_array = new char*[args.size() + 1];

	args_array[0] = copy_c_str(cmd.first);
        for (size_t i = 0; i < args.size(); ++i) {
            args_array[i + 1] = copy_c_str(args[i]);
        }

        execvp(args_array[0], args_array);

        for (size_t i = 0; i <= args.size(); ++i) {
            delete[] args_array[i];;
        }
        delete[] args_array;
        
        exit(EXIT_SUCCESS);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    struct sigaction act;
    act.sa_handler = &sigint_handler;
    sigaction(SIGINT, &act, NULL);

    while (true) {
        cout << "> ";
        cout.flush();

        string s = "";
        if (getline(cin, s)) {
            auto cmd = split_string(s);

            bool command_is_found = false;
            for (auto it = utils.begin(); it != utils.end(); ++it) {
                if (cmd.first == it->first) {
                    it->second(cmd.second);
                    command_is_found = true;
                }
            }

            if (!command_is_found) {
                exec_handler(cmd);
            }
        }
    }

    return 0;
}
