#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <sstream>
#include <vector>

inline std::vector<string> split(const string & str, const char delimiter) {
    std::stringstream test(str);
    std::string segment;
    std::vector<string> seglist;

    while(std::getline(test, segment, delimiter)) {
       seglist.push_back(segment);
    }

    return seglist;
}

#endif // STRING_UTILS_H
