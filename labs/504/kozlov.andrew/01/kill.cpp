#include "common.h"

void kill_handler(arguments args) {
    if (args.size() == 2) {
        int pid = stoi(args[0]);
        if (pid < 0) {
            print_with_prefix_and_suffix("kill", "pid shold be an integer number greater than 0");
            return;
        }
        if (errno != 0) {
            print_error("kill");
            return;
        }

        int signal = stoi(args[1]);
        if (errno != 0) {
            print_error("kill");
            return;
        }
        if (signal < 0 || signal > 64) {
            print_with_prefix_and_suffix("kill", "signal shold be an integer number in range between 0 and 64");
            return;
        }

        if (kill(pid, signal) != 0) {
            print_error("kill");
        }
    } else {
        print_with_prefix_and_suffix("kill", "usage: kill <pid> <signal>");
    }
}