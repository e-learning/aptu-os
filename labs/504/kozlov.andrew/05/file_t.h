#pragma once

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>

using std::string;

class file_t {
public:
    static std::pair<std::vector<string>, string> split_path(const string&);
    static bool name_is_valid(const string &);

    file_t();
    file_t(const string&, uint32_t, size_t);

    string name() const;
    void set_name(const string&);

    uint32_t start_block() const;
    size_t size() const;

    bool is_valid() const;

    string info(size_t) const;

    friend std::istream &operator>>(std::istream&, file_t&);
    friend std::ostream &operator<<(std::ostream&, const file_t&);

private:
    string name_;
    uint32_t start_block_;
    size_t size_;

    static const size_t MAX_NAME_LENGTH = 10;
};
