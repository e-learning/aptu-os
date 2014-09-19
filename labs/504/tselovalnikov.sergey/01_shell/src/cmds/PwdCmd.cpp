#include <cstdio>
#include <unistd.h>
#include <string>
#include <string.h>

#include "PwdCmd.h"

string PwdCmd::exec() {
    char the_path[256];

    getcwd(the_path, 255);
    strcat(the_path, "/");

    return the_path;
}
