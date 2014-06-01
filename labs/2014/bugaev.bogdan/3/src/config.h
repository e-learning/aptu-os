#ifndef CONFIG_H
#define CONFIG_H

#include "exception.h"
#include "path.h"
#include <string>



CREATE_EXCEPTION(ConfigException)

struct Config
{
    Path root;
    int pageSize;
    int pageCount;
};

Config const init(int argc, char **argv);


#endif // CONFIG_H
