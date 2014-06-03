#ifndef FILE_HPP
#define FILE_HPP

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <iomanip>
using namespace std;

#include "types.hpp"
#include "utils.hpp"


class file {
public:
    static pair<vector<string>, string> split_path(const string &path);

public:
    file() {};
    file(const string &name, block_num start_block, bytes size)
        : m_name(name), m_start_block(start_block), m_size(size)
        , m_ctime(time(nullptr))
        {}

    friend istream &operator>>(istream &in, file &f);
    friend ostream &operator<<(ostream &out, const file &f);

    const string &name() const
        { return m_name; }

    void set_name(const string &n)
        { m_name = n; }

    void update_ctime()
        { m_ctime = time(nullptr); }

    bool is_valid() const
        { return !m_name.empty(); }

    string info(bytes block_size) const;

    block_num start_block() const
        { return m_start_block; }

    bytes size() const
        { return m_size; }

private:
    string m_name;
    block_num m_start_block;
    bytes m_size;
    time_t m_ctime;
};

#endif /* end of include guard: FILE_HPP */
