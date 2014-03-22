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

string file::info() const
{
    ostringstream ss;
    ss << m_name << endl;
    ss << m_size << " bytes" << endl;

    char buf[128];
    time_t t = m_ctime;
    struct tm *timeinfo = localtime(&t);
    strftime(buf, 128, "%c", timeinfo);
    ss << buf << endl;
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
