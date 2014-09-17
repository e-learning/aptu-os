#include "common.h"

#include <array>
#include <sstream>
#include <stdlib.h>

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
    exit(0);
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
            auto c = split_string(s);

            bool command_is_found = false;
            for (auto it = utils.begin(); it != utils.end(); ++it) {
                if (c.first == it->first) {
                    it->second(c.second);
                    command_is_found = true;
                }
            }

            if (!command_is_found) {

            }
        }
    }

    return 0;
}
