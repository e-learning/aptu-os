#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <sstream>
#include <vector>

using std::string;
using std::vector;
using std::getline;
using std::stringstream;

inline vector<string> split (const string & str, const char delimiter) {
    stringstream test(str);
    string segment;
    vector<string> seglist;

    while(getline(test, segment, delimiter)) {
       seglist.push_back(segment);
    }

    return seglist;
}


#endif // STRING_UTILS_H
