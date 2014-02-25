#ifndef STRING_OPS_H
#define STRING_OPS_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <string>
#include <iterator>

inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

inline std::vector<std::string> split(std::string input){
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
             std::istream_iterator<std::string>(),
             std::back_inserter<std::vector<std::string> >(tokens));
    return tokens;
}

#endif // STRING_OPS_H
