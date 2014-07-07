#include "file.hpp"

pair<vector<string>, string>
file::split_path(const string &path)
{
    if (path.compare("/") == 0)
        return make_pair(vector<string>(), "");
    string s(path);
    replace(s.begin(), s.end(), '/', ' ');
    istringstream ss(s);
    vector<string> result(
            (istream_iterator<string>(ss)),
             istream_iterator<string>());
    string filename = result.back();
    result.pop_back();
    return make_pair(result, filename);
}

string file::info(bytes block_size) const
{
    ostringstream ss;
    ss << "F | ";
    ss << setw(11) << m_name << " | ";
    ss << setw(6) << m_size / block_size << " | ";
    ss << time_to_string(m_ctime) << endl;
    return ss.str();
}

istream &operator>>(istream &in, file &f)
{
    in >> f.m_name;
    in >> f.m_start_block;
    in >> f.m_size;
    in >> f.m_ctime;
    return in;
}

ostream &operator<<(ostream &out, const file &f)
{
    out << f.m_name << " ";
    out << f.m_start_block << " ";
    out << f.m_size << " ";
    out << f.m_ctime;
    return out << endl;
}
