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

string directory::info(bytes block_size) const
{
    ostringstream ss;
    ss << "D | " << setw(11) << m_name
       << " |      0 | " << time_to_string(m_ctime) << endl;
    for (auto &entry: m_dirs)
        ss << "D | " << setw(11) << entry.first
           << " |      0 | " << time_to_string(entry.second.m_ctime) << endl;
    for (auto &entry: m_files)
        ss << entry.second.info(block_size);
    return ss.str();
}

bool directory::is_parent(directory *child) const
{
    while (child && child != this)
        child = child->parent();
    return child == this;
}

vector<file> directory::files()
{
    vector<file> results;
    for (auto &f: m_files)
        results.push_back(f.second);
    return results;
}

vector<directory> directory::dirs()
{
    vector<directory> results;
    for (auto &d: m_dirs)
        results.push_back(d.second);
    return results;
}

istream &operator>>(istream &in, directory &d)
{
    in >> d.m_name;
    in >> d.m_ctime;
    size_t dir_count  = 0;
    size_t file_count = 0;
    in >> dir_count >> file_count;
    for (size_t i = 0; i < dir_count; ++i) {
        directory c;
        in >> c;
        d.m_dirs[c.name()] = c;
        d.m_dirs[c.name()].set_parent(&d);
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
    out << d.m_name  << " ";
    out << d.m_ctime << " ";
    out << d.m_dirs.size() << " ";
    out << d.m_files.size() << endl;
    for (auto &entry: d.m_dirs)
        out << entry.second;
    for (auto &entry: d.m_files)
        out << entry.second;
    return out;
}

