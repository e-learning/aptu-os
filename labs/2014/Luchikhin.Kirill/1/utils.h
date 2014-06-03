#ifndef UTILS_H
#define UTILS_H

#include <string>

using std::string;

namespace utils{
    string &rtrim(string &src);
    string &ltrim(string &src);
    string &trim(string &src);
}

#endif // UTILS_H
