#ifndef HEADER_H
#define HEADER_H

#include "config.h"


int popFreePageNumber(Config const &config);

int pushFreePageNumber(Config const &config, int pageNumber);

int getHeaderSize(Config const &config);

int getFreePageCount(Config const &config);


#endif // HEADER_H
