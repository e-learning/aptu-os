#include "fs_t.h"
#include "file_buffer.h"

fs_t::fs_error::fs_error(const string& message)
        : runtime_error(message) {
}

fs_t::fs_t(const string &path, bool pass_metadata)
        : path_(path)
        , block_size_(0)
        , blocks_count_(0) {
    path_ = path_.back() == '/' ? path_ : path_ + "/";
    read_config_file();

    if (!pass_metadata) {
        read_metadata_file();

        if (is_formatted()) {
            read_bitmap_file();
        }
    }
}

fs_t::~fs_t() {
    if (!uncaught_exception()) {
        write_metadata_file();
        write_bitmap_file();
    }
}

bool fs_t::is_valid() const {
    return blocks_count_ > 0 && block_size_ >= MIN_BLOCK_SIZE;
}

bool fs_t::is_formatted() const {
    return is_valid() && root_.is_valid();
}

size_t fs_t::block_size() const {
    return block_size_;
}

string fs_t::block_path(uint32_t index) {
    return path_ + to_string(index);
}

void fs_t::init() {
    for (size_t i = 0; i < blocks_count_; i++) {
        ofstream out(path_ + to_string(i), ios::trunc);
        out.flush();
    }
}

void fs_t::format() {
    init();

    root_ = directory_t("/");
    bitmap_.assign(blocks_count_, '\0');
}

void fs_t::import_(const string& from_path, const string &to_path) {
    ifstream in(from_path, ios::binary);

    auto split_path = file_t::split_path(to_path);
    string filename = split_path.second;
    if (!file_t::name_is_valid(filename)) {
        throw new fs_error("invalid file name");
    }

    directory_t* to = find_last(split_path.first);
    if (!to) {
        throw fs_error("no such directory");
    }

    uint32_t start_block = next_free_block();
    if (start_block == NO_FREE_BLOCKS) {
        throw fs_error("no free blocks");
    }

    out_file_buffer buf(this);
    ostream out(&buf);
    out << in.rdbuf();

    in.seekg(0, in.end);
    size_t size = in.tellg();

    file_t f(filename, start_block, size);
    to->add_file(f);
}

void fs_t::export_(const string &from_path, const string &to_path) {
    ofstream out(to_path, ios::binary | ios::trunc);

    auto split_path = file_t::split_path(from_path);
    directory_t* from = find_last(split_path.first);
    if (!from) {
        throw fs_error("no such directory");
    }

    file_t* file = from->find_file(split_path.second);
    if (!file) {
        throw fs_error("no such file");
    }

    in_file_buffer buf(this, *file);
    istream in(&buf);
    out << in.rdbuf();
}

string fs_t::ls(const string &path) {
    auto split_path = file_t::split_path(path);
    directory_t* item = find_last(split_path.first);
    if (!item) {
        throw fs_error("no such item");
    }

    directory_t* directory = item;
    if (!split_path.second.empty()) {
        directory = item->find_directory(split_path.second);
    }

    if (directory) {
        return directory->info(block_size_);
    }

    file_t* file = item->find_file(split_path.second);
    if (file) {
        return file->info(block_size_);
    } else {
        throw fs_error("no such item");
    }
}

void fs_t::copy(const string& src_path, const string& dst_path) {
    directory_t* src;
    string src_name;

    directory_t* dst;
    string dst_name;
    tie(src, dst, src_name, dst_name) = find_roots(src_path, dst_path);

    directory_t* directory = src->find_directory(src_name);
    if (directory) {
        return copy_directory(directory, dst_name, src, dst);
    }

    file_t* file = src->find_file(src_name);
    if (file) {
        return copy_file(file, dst_name, dst);
    } else {
        throw fs_error("no such item");
    }
}

void fs_t::move(const string& src_path, const string& dst_path) {
    directory_t* src;
    string src_name;

    directory_t* dst;
    string dst_name;
    tie(src, dst, src_name, dst_name) = find_roots(src_path, dst_path);

    directory_t* directory = src->find_directory(src_name);
    if (directory) {
        if (directory->is_parent(dst)) {
            throw fs_error("moving directory into itself is forbidden");
        }

        directory_t new_directory(*directory);
        new_directory.set_name(dst_name);

        dst->add_directory(new_directory);
        src->remove_directory(directory->name());

        return;
    }

    file_t* file = src->find_file(src_name);
    if (file) {
        file_t new_file(*file);
        new_file.set_name(dst_name);

        dst->add_file(new_file);
        src->remove_file(file->name());
    } else {
        throw fs_error("no such item");
    }
}

void fs_t::rm(const string &path) {
    auto splits = file_t::split_path(path);

    directory_t *root = find_last(splits.first);
    if (!root) {
        throw fs_error("no such item");
    }

    if (splits.second.empty()) {
        for (directory_t& directory: root->directories()) {
            remove_directory(&directory);
            root->remove_directory(directory.name());
        }

        for (file_t& file: root->files()) {
            remove_file(&file);
            root->remove_file(file.name());
        }

        return;
    }

    directory_t* directory = root->find_directory(splits.second);
    if (directory) {
        remove_directory(directory);
        return root->remove_directory(directory->name());
    }

    file_t* file = root->find_file(splits.second);
    if (file) {
        remove_file(file);
        return root->remove_file(file->name());
    } else {
        throw fs_error("no such item");
    }
}

void fs_t::mkdir(const string &path) {
    auto split_path = file_t::split_path(path);
    directory_t* directory = &root_;
    bool directory_found = false;

    for (const string &dir: split_path.first) {
        if (!directory_found) {
            directory_t* previous = directory;
            directory = directory->find_directory(dir);

            if (!directory) {
                directory_found = true;
                directory = previous;
            }
        }

        if (directory_found) {
            if (!file_t::name_is_valid(dir)) {
                throw new fs_error("wrong item name");
            }

            if (directory->find_file(dir)) {
                throw fs_error("item already exists");
            }

            directory = directory->add_directory(directory_t(dir));
        }
    }

    if (!file_t::name_is_valid(split_path.second)) {
        throw new fs_error("wrong item name");
    }

    if (directory->find_file(split_path.second)) {
        throw fs_error("item already exists");
    }

    directory->add_directory(directory_t(split_path.second));
}

tuple<directory_t*, directory_t*, string, string> fs_t::find_roots(const string &src_path, const string &dst_path) {
    auto src_split_path = file_t::split_path(src_path);
    directory_t* src_root = find_last(src_split_path.first);
    if (!src_root) {
        throw fs_error("no such item");
    }

    auto dst_split_path = file_t::split_path(dst_path);
    if (!file_t::name_is_valid(dst_split_path.second)) {
        throw fs_error("wrong file name");
    }

    directory_t*dst_root = find_last(dst_split_path.first);
    if (!dst_root) {
        throw fs_error("no such item");
    }

    if (src_split_path.second.compare(dst_split_path.second) == 0 && dst_root == src_root) {
        throw fs_error("copying is forbidden");
    }

    if (dst_root->find_file(dst_split_path.second)) {
        throw fs_error("item already exists");
    }

    if (dst_root->find_directory(dst_split_path.second)) {
        dst_root = dst_root->find_directory(dst_split_path.second);
        dst_split_path.second = src_split_path.second;

        if (dst_root->find_directory(dst_split_path.second) || dst_root->find_file(dst_split_path.second)) {
            throw fs_error("item already exists");
        }
    }

    return make_tuple(src_root, dst_root, src_split_path.second, dst_split_path.second);
}

void fs_t::copy_directory(directory_t *directory, const string& name, directory_t* src, directory_t *dst) {
    directory_t *new_directory = dst->add_directory(directory_t(name));
    for(file_t& file: directory->files())
        copy_file(&file, file.name(), new_directory);

    for (directory_t& temp: directory->directories()) {
        copy_directory(&temp, temp.name(), directory, new_directory);
    }
}

void fs_t::copy_file(file_t* file, const string& name, directory_t* dst) {
    uint32_t start_block = next_free_block();
    if (start_block == NO_FREE_BLOCKS) {
        throw fs_error("no free blocks are available");
    }

    in_file_buffer in_buffer(this, *file);
    istream in(&in_buffer);

    out_file_buffer out_buffer(this);
    ostream out(&out_buffer);

    out << in.rdbuf();

    file_t new_file(name, start_block, file->size());
    dst->add_file(new_file);
}

void fs_t::remove_file(file_t* file) {
    in_file_buffer in_buffer(this, *file, true);
    istream in(&in_buffer);

    ofstream out("/dev/null");
    out << in.rdbuf();

    for (uint32_t i: in_buffer.blocks()) {
        bitmap_[i] = FREE;
    }
}

void fs_t::remove_directory(directory_t* directory) {
    for (file_t& file: directory->files()) {
        remove_file(&file);
    }
    for (directory_t& temp: directory->directories()) {
        remove_directory(&temp);
    }
}

uint32_t fs_t::next_free_block() const {
    if (!is_formatted()) {
        return NO_FREE_BLOCKS;
    }

    vector<char>::const_iterator match = find(bitmap_.begin(), bitmap_.end(), 0);
    return (match == end(bitmap_)) ? NO_FREE_BLOCKS : match - begin(bitmap_);
}

void fs_t::mark_block(uint32_t index, char value) {
    bitmap_[index] = value;
}

directory_t* fs_t::find_last(vector<string> path) {
    directory_t* result = &root_;

    for (const string& directory: path) {
        result = result->find_directory(directory);

        if (!result) {
            return nullptr;
        }
    }

    return result;
}

void fs_t::read_config_file() {
    ifstream in(path_ + CONFIG_FILENAME);

    in >> block_size_ >> blocks_count_;
    blocks_count_ -= 2;

    if (!is_valid()) {
        throw fs_error("wrong config file");
    }
}

void fs_t::read_metadata_file() {
    ifstream in(path_ + to_string(blocks_count_));

    if (in.is_open()) {
        in >> root_;
    }
}

void fs_t::read_bitmap_file() {
    ifstream in(path_ + to_string(blocks_count_ + 1), ios::binary);
    bitmap_.assign(istream_iterator<char>(in),
                    istream_iterator<char>());

    if (bitmap_.size() != blocks_count_) {
        throw fs_error("wrong bitmap file size");
    }
}

void fs_t::write_metadata_file() {
    ofstream out(path_ + to_string(blocks_count_), ios::trunc);

    out << root_;
}

void fs_t::write_bitmap_file() {
    ofstream out(path_ + to_string(blocks_count_ + 1), ios::trunc | ios::binary);

    ostream_iterator<char> out_it(out);
    std::copy(bitmap_.begin(), bitmap_.end(), out_it);
}
