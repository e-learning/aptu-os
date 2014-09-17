#include "common.h"

#include <fstream>
#include <iomanip>

int get_pid(const char *entry_name) {
    int pid;

    if (sscanf(entry_name, "%u", &pid)) {
        return pid;
    } else {
        return -1;
    }
}

typedef pair<const string, const string> row;

void print_table_cell(const string &s, int width) {
    cout << setw(width) << std::left << s;
}

void print_table_row(const row &r) {
    print_table_cell(r.first, 8);
    cout << " ";

    print_table_cell(r.second, 16);
    cout << endl;
}

row get_info(const string &pid) {
    ifstream status("/proc/" + pid + "/status");

    string name;
    status >> name;
    status >> name;

    return make_pair(pid, name);
}

void ps_handler(arguments) {
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) {
        print_error("ps");
        return;
    }

    vector <string> pids;

    dirent *dir_entry = readdir(proc_dir);
    while (dir_entry) {
        char *pid = dir_entry->d_name;

        if (get_pid(pid) != -1) {
            pids.push_back(string(pid));
        }

        dir_entry = readdir(proc_dir);
    }

    closedir(proc_dir);

    print_table_row(make_pair("PID", "NAME"));
    for (auto it = pids.begin(); it != pids.end(); ++it) {
        print_table_row(get_info(*it));
    }
}