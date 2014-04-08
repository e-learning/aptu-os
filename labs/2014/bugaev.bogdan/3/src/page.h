#ifndef PAGE_H
#define PAGE_H

#include "exception.h"
#include "config.h"
#include "defs.h"
#include <functional>
#include <algorithm>
#include <cassert>


CREATE_EXCEPTION(PageException)

class Page
{
public:
    Page():
        m_size(0),
        m_data(0),
        m_fd(0)
    {
    }

    char *data()
    {
        return m_data;
    }

    int size() const
    {
        return m_size;
    }

    void load(Config const &config, int number);
    void unload();

private:
    Page(Page const &);
    Page &operator =(Page const &);

private:
    int m_size;
    char *m_data;
    int m_fd;
};


class PageWriter
{
public:
    explicit PageWriter(Page &page):
        m_page(page),
        m_pos(page.data())
    {
    }

    template<class T>
    bool canWrite() const
    {
        return (m_page.data() + m_page.size()) - m_pos >= sizeof(T);
    }

    bool canWrite(std::string const &str) const
    {
        return (m_page.data() + m_page.size()) - m_pos >=
               static_cast<int>(sizeof(char) * str.size());
    }

    template<class T>
    PageWriter &write(T const &value)
    {
        assert(canWrite<T>());
        *reinterpret_cast<T *>(m_pos) = value;
        m_pos += sizeof(T);
        return *this;
    }

    PageWriter &writeString(std::string const &str)
    {
        assert(canWrite(str));
        for (size_t i = 0; i < str.size(); ++i) {
            write(str[i]);
        }
        int d = LEN - str.size();
        while (d--) {
            write<char>(0);
        }
        return *this;
    }

    template<class T>
    PageWriter &step(int n = 1)
    {
        assert(n >= 0);
        m_pos += std::min<int>(sizeof(T) * n,
                               (m_page.data() + m_page.size()) - m_pos);
        return *this;
    }

private:
    PageWriter(PageWriter const &);
    PageWriter &operator =(PageWriter const &);

private:
    Page &m_page;
    char *m_pos;
};


class PageReader
{
public:
    explicit PageReader(Page &page):
        m_page(page),
        m_pos(page.data())
    {
    }

    template<class T>
    bool canRead() const
    {
        return (m_page.data() + m_page.size()) - m_pos >= sizeof(T);
    }

    template<class T>
    T const read()
    {
        m_pos += sizeof(T);
        return *reinterpret_cast<T *>(m_pos - sizeof(T));
    }

    template<class T>
    PageReader &step(int n = 1)
    {
        assert(n >= 0);
        m_pos += std::min<int>(sizeof(T) * n,
                               (m_page.data() + m_page.size()) - m_pos);
        return *this;
    }

private:
    PageReader(PageReader const &);
    PageReader &operator =(PageReader const &);

private:
    Page &m_page;
    char *m_pos;
};


#endif // PAGE_H
