#pragma once

#include <algorithm>
#include <vector>
#include <sstream>

namespace utils {
    static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
    }

    static inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            if(!item.empty()) {
                elems.push_back(item);
            }
        }
        return elems;
    }

    static inline std::string join(const std::vector<std::string> &elements, const std::string &delimeter) {
        if (!elements.empty())
        {
            std::stringstream ss;
            unsigned long size = elements.size();
            for (int i = 0; i < size; i++) {
                ss << elements[i];
                if(i != size - 1) {
                    ss << delimeter;
                }
            }
            return ss.str();
        }
        return "";
    }
}