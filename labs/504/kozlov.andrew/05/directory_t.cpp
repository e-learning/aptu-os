#include "directory_t.h"

directory_t::directory_t() {
}

directory_t::directory_t(const string& name)
        : name_(name)
        , parent_(nullptr) {
}

directory_t::directory_t(const directory_t& directory) {
    name_ = directory.name_;
    parent_ = directory.parent_;

    files_ = directory.files_;
    directories_ = directory.directories_;

    for (auto& dir: directories_) {
        dir.second.set_parent(this);
    }
}

directory_t& directory_t::operator=(const directory_t& directory) {
    name_ = directory.name_;
    parent_ = directory.parent_;

    files_ = directory.files_;
    directories_ = directory.directories_;

    for (auto& dir: directories_) {
        dir.second.set_parent(this);
    }

    return *this;
}

string directory_t::name() const {
    return name_;
}

void directory_t::set_name(const string& name) {
    name_ = name;
}

directory_t *directory_t::parent() const {
    return parent_;
}

void directory_t::set_parent(directory_t* parent) {
    parent_ = parent;
}

bool directory_t::is_valid() const {
    if (name_.empty()) {
        return false;
    } else {
        return name_.size() <= MAX_NAME_LENGTH;
    }
}

bool directory_t::is_parent(directory_t* directory) const {
    while (directory != NULL && directory != this) {
        directory = directory->parent();
    }

    return directory == this;
}

file_t* directory_t::find_file(const string& file) {
    std::map<string, file_t>::iterator match = files_.find(file);

    return match == files_.end() ? nullptr : &(match->second);
}

directory_t* directory_t::find_directory(const string& directory) {
    std::map<string, directory_t>::iterator match = directories_.find(directory);

    return match == directories_.end() ? nullptr : &(match->second);
}

void directory_t::add_file(const file_t& file) {
    files_[file.name()] = file;
}

directory_t* directory_t::add_directory(const directory_t& directory) {
    string name = directory.name();

    directories_[name] = directory;
    directories_[name].set_parent(this);

    return &(directories_[name]);
}

void directory_t::remove_file(const string& file) {
    files_.erase(file);
}

void directory_t::remove_directory(const string& directory) {
    directories_.erase(directory);
}

vector<file_t> directory_t::files() {
    vector<file_t> results;
    for (auto& file: files_) {
        results.push_back(file.second);
    }

    return results;
}

vector<directory_t> directory_t::directories() {
    vector<directory_t> results;
    for (auto& directory: directories_) {
        results.push_back(directory.second);
    }

    return results;
}

string directory_t::info(size_t block_size) const {
    std::ostringstream ss;
    ss << std::setw(MAX_NAME_LENGTH + 1) << name_ << " d 0" << std::endl;

    for (auto& entry: files_) {
        ss << entry.second.info(block_size);
    }
    for (auto& entry: directories_) {
        ss << std::setw(MAX_NAME_LENGTH + 1) << entry.first << " d 0" << std::endl;
    }

    return ss.str();
}

std::istream &operator>>(std::istream& in, directory_t& directory) {
    in >> directory.name_;

    size_t dir_count  = 0;
    size_t file_t_count = 0;
    in >> dir_count >> file_t_count;

    for (size_t i = 0; i < dir_count; i++) {
        directory_t c;
        in >> c;

        directory.directories_[c.name()] = c;
        directory.directories_[c.name()].set_parent(&directory);
    }
    for (size_t i = 0; i < file_t_count; i++) {
        file_t f;
        in >> f;

        directory.files_[f.name()] = f;
    }

    return in;
}

std::ostream &operator<<(std::ostream& out, const directory_t& directory) {
    out << directory.name_ << " ";

    out << directory.directories_.size() << " ";
    out << directory.files_.size() << std::endl;

    for (auto& entry: directory.directories_) {
        out << entry.second;
    }
    for (auto& entry: directory.files_) {
        out << entry.second;
    }

    return out;
}

