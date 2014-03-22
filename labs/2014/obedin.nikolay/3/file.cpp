#include "file.hpp"


istream &operator>>(istream &in, file &f)
{
    in >> f.m_name;
    in >> f.m_start_pos.block_n;
    in >> f.m_start_pos.offset;
    in >> f.m_size;
    in >> f.m_ctime;
    return in;
}

ostream &operator<<(ostream &out, file &f)
{
    out << f.m_name;
    out << f.m_start_pos.block_n;
    out << f.m_start_pos.offset;
    out << f.m_size;
    out << f.m_ctime;
    return out;
}
