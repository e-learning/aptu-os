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
    explicit directory(const string &name): m_name(name) {};

    const string &name() const
        { return m_name; }

    bool is_valid() const
        { return !m_name.empty(); }

    directory *find_child_dir(const string &name);
    file *find_child_file(const string &name);

    void add_child(const file &f)
        { m_files[f.name()] = f; }

    string info() const;

    friend istream &operator>>(istream &in, directory &d);
    friend ostream &operator<<(ostream &out, const directory &d);

private:
    string m_name;
    filemap m_files;
    dirmap m_dirs;
};


#endif /* end of include guard: DIRECTORY_HPP */
