#include "common.h"

void print_dir(const string &path) {
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        print_error(path);
        return;
    }

    dirent *dir_entry = readdir(dir);
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