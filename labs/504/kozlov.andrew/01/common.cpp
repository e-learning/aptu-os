#include "common.h"

void print_with_prefix_and_suffix(const string &str, const string &message) {
    cout << str << ": ";

    if (!message.empty()) {
        cout << message;
    }

    cout << endl;
}

void print_error(const string &cmd) {
    print_with_prefix_and_suffix(cmd, strerror(errno));
}