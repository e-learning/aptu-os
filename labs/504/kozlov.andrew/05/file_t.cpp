#include "file_t.h"

std::pair<std::vector<string>, string> file_t::split_path(const string& path) {
    if (path.compare("/") == 0) {
        return make_pair(std::vector<string>(), "");
    }

    string s(path);
    replace(s.begin(), s.end(), '/', ' ');

    std::istringstream ss(s);
    std::vector<string> result(
            (std::istream_iterator<string>(ss)),
    std::istream_iterator<string>());

    string filename = result.back();
    result.pop_back();

    return make_pair(result, filename);
}

bool file_t::name_is_valid(const string& item) {
    if (item.empty()) {
        return false;
    } else {
        return item.size() <= MAX_NAME_LENGTH;
    }
}

file_t::file_t() {
}

file_t::file_t(const string& name, uint32_t start_block, size_t size)
        : name_(name)
        , start_block_(start_block)
        , size_(size) {
}

string file_t::name() const {
    return name_;
}

void file_t::set_name(const string& name) {
    name_ = name;
}

uint32_t file_t::start_block() const {
    return start_block_;
}

size_t file_t::size() const {
    return size_;
}

bool file_t::is_valid() const {
    return name_is_valid(name_);
}

string file_t::info(size_t block_size) const {
    std::ostringstream ss;
    ss << std::setw(MAX_NAME_LENGTH + 1) << name_ << " f " << size_ / block_size << std::endl;

    return ss.str();
}

std::istream &operator>>(std::istream& in, file_t& file) {
    in >> file.name_ >> file.start_block_ >> file.size_;

    return in;
}

std::ostream &operator<<(std::ostream &out, const file_t& file) {
    out << file.name_ << " " << file.start_block_ << " " << file.size_ << std::endl;

    return out;
}
