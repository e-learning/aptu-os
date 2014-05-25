#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <string>


template<class T>
std::string const getString(T const &value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}


#endif // UTILS_H
