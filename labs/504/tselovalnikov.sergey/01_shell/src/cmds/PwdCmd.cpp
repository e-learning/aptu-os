#include <cstdio>
#include <unistd.h>
#include <string>
#include <string.h>

#include "PwdCmd.h"

void PwdCmd::exec() {
    char the_path[256];
    getcwd(the_path, 255);
    strcat(the_path, "/");
    printf("%s\n", the_path);
}
