#include "path.h"
#include <cctype>


void Path::init(std::string const &path)
{
    size_t i = 0;
    while (i < path.size() && isspace(path[i])) {
        ++i;
    }

    m_absolute = path[i] == '/';
    i += m_absolute;

    std::string word;
    for (; i < path.size(); ++i) {
        if (path[i] == '/') {
            m_data.push_back(word);
            word.clear();
        } else {
            word += path[i];
        }
    }
    if (!word.empty()) {
        m_data.push_back(word);
    }
}


std::string const Path::getString() const
{
    std::string result;
    for (size_t i = 0; i < m_data.size(); ++i) {
        if (i || m_absolute) {
            result += '/';
        }
        result += m_data[i];
    }
    return result;
}
