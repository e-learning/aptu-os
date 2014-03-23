#include "directory.hpp"


directory *directory::find_child_dir(const string &name)
{
    dirmap::iterator match = m_dirs.find(name);
    if (match == m_dirs.end())
        return nullptr;
    return &(match->second);
}

file *directory::find_child_file(const string &name)
{
    filemap::iterator match = m_files.find(name);
    if (match == m_files.end())
        return nullptr;
    return &(match->second);
}

string directory::info() const
{
    ostringstream ss;
    for (auto &entry: m_dirs)
        ss << "D " << entry.first << endl;
    for (auto &entry: m_files)
        ss << "F " << entry.first << endl;
    return ss.str();
}

istream &operator>>(istream &in, directory &d)
{
    in >> d.m_name;
    size_t dir_count  = 0;
    size_t file_count = 0;
    in >> dir_count >> file_count;
    for (size_t i = 0; i < dir_count; ++i) {
        directory c;
        in >> c;
        d.m_dirs[c.name()] = c;
    }
    for (size_t i = 0; i < file_count; ++i) {
        file f;
        in >> f;
        d.m_files[f.name()] = f;
    }
    return in;
}

ostream &operator<<(ostream &out, const directory &d)
{
    out << d.m_name << " ";
    out << d.m_dirs.size() << " ";
    out << d.m_files.size() << endl;
    for (auto &entry: d.m_dirs)
        out << entry.second;
    for (auto &entry: d.m_files)
        out << entry.second;
    return out;
}

