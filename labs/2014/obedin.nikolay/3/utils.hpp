#ifndef UTILS_HPP
#define UTILS_HPP

#include <ctime>

inline string time_to_string(time_t t) {
    char buf[128];
    struct tm *timeinfo = localtime(&t);
    strftime(buf, 128, "%D %T", timeinfo);
    return string(buf);
}

#endif /* end of include guard: UTILS_HPP */
