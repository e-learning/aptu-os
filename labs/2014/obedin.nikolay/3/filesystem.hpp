#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <fstream>
#include <exception>
using namespace std;

#include "types.hpp"

class filesystem {

public:
    explicit filesystem(const string &path);
    bool is_valid() const;
    void init();
    void format() {};

private:
    string m_path;
    bytes m_block_size;
    size_t m_blocks_count;

public:
    struct error: runtime_error
    { explicit error(const string &m) : runtime_error(m) {} };

};

#endif /* end of include guard: FILESYSTEM_HPP */
