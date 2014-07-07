#ifndef SHELL_H
#define SHELL_H

#include "directory.h"

#include <unistd.h>

struct Shell{
    Shell():
        curDir(get_current_dir_name())
    {}

    void run();
private:
    Directory curDir;

    bool executeCommand(string const command);
    void ls();
    void pwd();
    void ps();
    void killF(string command);
};

#endif // SHELL_H
