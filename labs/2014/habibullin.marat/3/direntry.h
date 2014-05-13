#ifndef DIRENTRY_H
#define DIRENTRY_H

#include <string>

struct DirEntry {
    std::string name;
    size_t ptr;
    const static size_t SIZE = 14;

    DirEntry(std::string const& n, size_t p): name(n), ptr(p) {}
};

#endif // DIRENTRY_H
