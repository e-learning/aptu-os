#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include <ios>
#include <map>
#include <string>
using namespace std;

#include "types.hpp"
#include "file.hpp"


class directory;

typedef map<string, file> filemap;
typedef map<string, directory> dirmap;


class directory {
public:
    directory() {};
    explicit directory(const string &name)
        : m_name(name), m_parent(nullptr)
        {}

    directory(const directory &d)
    {
        m_name   = d.m_name;
        m_parent = d.m_parent;
        m_files  = d.m_files;
        m_dirs   = d.m_dirs;
        for (auto &d: m_dirs)
            d.second.set_parent(this);
    }

    directory &operator=(const directory &d)
    {
        m_name   = d.m_name;
        m_parent = d.m_parent;
        m_files  = d.m_files;
        m_dirs   = d.m_dirs;
        for (auto &d: m_dirs)
            d.second.set_parent(this);
        return *this;
    }

    const string &name() const
        { return m_name; }

    void set_name(const string &n)
        { m_name = n; }

    directory *parent() const
        { return m_parent; }

    void set_parent(directory *d)
        { m_parent = d; }

    bool is_valid() const
        { return !m_name.empty(); }

    bool is_parent(directory *child) const;

    directory *find_child_dir(const string &name);
    file *find_child_file(const string &name);

    void add_child_file(const file &f)
        { m_files[f.name()] = f; }

    directory *add_child_dir(const directory &d)
    {
        m_dirs[d.name()] = d;
        m_dirs[d.name()].set_parent(this);
        return &(m_dirs[d.name()]);
    }

    void remove_child_dir(const string &name)
        { m_dirs.erase(name); }

    void remove_child_file(const string &name)
        { m_files.erase(name); }

    string info() const;
    vector<file> files();
    vector<directory> dirs();

    friend istream &operator>>(istream &in, directory &d);
    friend ostream &operator<<(ostream &out, const directory &d);

private:
    string m_name;
    directory *m_parent;
    filemap m_files;
    dirmap m_dirs;
};


#endif /* end of include guard: DIRECTORY_HPP */
