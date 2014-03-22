#ifndef FILE_HPP
#define FILE_HPP

#include <istream>
#include <ostream>
using namespace std;

#include "types.hpp"


class file {
public:
    struct pos { size_t block_n; bytes offset; };

    file() {};
    explicit file(const string &m_name);

    friend istream &operator>>(istream &in, file &f);
    friend ostream &operator<<(ostream &out, file &f);

private:
    string m_name;
    file::pos m_start_pos;
    bytes m_size;
    time_t m_ctime;
};

#endif /* end of include guard: FILE_HPP */
