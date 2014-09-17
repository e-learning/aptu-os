#include "common.h"

#include <unistd.h>
#include <sys/param.h>

void pwd_handler(arguments) {
    char result[MAXPATHLEN];
    getcwd(result, MAXPATHLEN);

    cout << result << endl;
}