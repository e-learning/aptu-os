#ifndef PATH_H
#define PATH_H

#include <vector>
#include <string>


class Path
{
public:
    Path():
        m_absolute(false)
    {
    }

    Path(std::string const &path)
    {
        init(path);
    }

    void init(std::string const &path);

    std::string const getString() const;

    std::vector<std::string> const &list() const
    {
        return m_data;
    }

    std::string const concat(std::string const &file) const
    {
        return getString() + '/' + file;
    }

    bool isAbsolute() const
    {
        return m_absolute;
    }

    bool isEmpty() const
    {
        return !m_absolute && m_data.empty();
    }

private:
    bool m_absolute;
    std::vector<std::string> m_data;
};


#endif // PATH_H
