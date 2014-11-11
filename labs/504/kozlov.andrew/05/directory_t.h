#pragma once

#include <ios>
#include <map>
#include <string>
#include <iomanip>

#include "file_t.h"

using std::string;
using std::vector;

class directory_t {
public:
    directory_t();
    explicit directory_t(const string&);

    directory_t(const directory_t&);
    directory_t& operator=(const directory_t&);

    string name() const;
    void set_name(const string&);

    directory_t* parent() const;
    void set_parent(directory_t*);

    bool is_valid() const;

    bool is_parent(directory_t*) const;

    file_t*find_file(const string &);
    directory_t*find_directory(const string &);
    void add_file(const file_t &);
    directory_t *add_directory(const directory_t &);
    void remove_directory(const string&);
    void remove_file(const string&);

    vector<file_t> files();
    vector<directory_t> directories();

    string info(size_t block_size) const;

    friend std::istream &operator>>(std::istream&, directory_t&);
    friend std::ostream &operator<<(std::ostream&, const directory_t&);

private:
    string name_;
    directory_t* parent_;

    std::map<string, file_t> files_;
    std::map<string, directory_t> directories_;

    static const size_t MAX_NAME_LENGTH = 10;
};