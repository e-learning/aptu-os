#include <iostream>
#include <sstream>
#include <climits>

struct buffer_t {
    buffer_t(const size_t size);
    ~buffer_t();

    uint8_t* begin() const;
    size_t size() const;

    uint16_t alloc(uint16_t size);
    bool free(uint16_t ptr);

    std::string info();
    std::string map();

private:
    size_t size_;
    uint8_t* begin_;

    size_t allocations_count;
    size_t used_bytes;
};

struct node_t {
    static const size_t RESERVED = 2;

    static void init_buffer(buffer_t* buffer) {
        node_t node(buffer, 0, buffer->size() - 1, buffer->size() - 2 * RESERVED, true);
        node.write();
    }

    node_t(buffer_t* buffer, const uint16_t begin=0) : buffer(buffer), begin_(begin), end_(0), size_(0), is_free_(false) {
        read_begin();
    }

    node_t(buffer_t* buffer, const size_t begin, const size_t end, const size_t size, const bool is_free) :
            buffer(buffer), begin_(begin), end_(end), size_(size), is_free_(is_free) {
    }

    uint16_t begin() const {
        return begin_;
    }

    uint16_t end() const {
        return end_;
    }

    uint16_t size() const {
        return size_;
    }

    bool is_free() const {
        return is_free_;
    }

    bool is_valid() const {
        return begin_ >= 0 && end_ >= 0 && begin_ < buffer->size() && end_ <= buffer->size();
    }

    void read_begin() {
        if (!is_valid()) {
            return;
        }

        read(buffer->begin() + begin_);
        end_ = begin_ + size_ + 2 * RESERVED;
    }

    void read_end() {
        if (!is_valid()) {
            return;
        }

        read(buffer->begin() + end_ - RESERVED);
        begin_ = end_ - size_ - 2 * RESERVED;
    }

    void write() const {
        write(buffer->begin() + begin_);
        write(buffer->begin() + end_ - RESERVED);
    }

    void next() {
        begin_ = end_;
        read_begin();
    }
private:
    buffer_t* buffer;
    uint16_t begin_, end_, size_;
    bool is_free_;

    void read(const uint8_t* ptr) {
        is_free_ = !(ptr[0] & (1 << 7));
        size_ = ptr[1];

        uint8_t begin = ptr[0] & (UCHAR_MAX ^ (1 << 7));
        size_ |= (begin << 8);
    }

    void write(uint8_t* ptr) const {
        ptr[0] = (uint8_t) (size_ >> 8);
        ptr[0] |= (!is_free_ << 7);

        ptr[1] = (uint8_t) size_;
    }
};

buffer_t::buffer_t(const size_t size) : size_(size), begin_(new uint8_t[size]), allocations_count(0), used_bytes(0) {
}

buffer_t::~buffer_t() {
    delete[] begin_;
}

uint8_t* buffer_t::begin() const {
    return begin_;
}

size_t buffer_t::size() const {
    return size_;
}

uint16_t buffer_t::alloc(uint16_t size) {
    node_t node(this);
    while (node.is_valid() && (!node.is_free() || node.size() < size)) {
        node.next();
    }

    if (!node.is_free() || !node.is_valid()) {
        return 0;
    }

    uint16_t delta = node.size() - size;
    uint16_t end = node.end() - delta;

    if (delta < 2 * node_t::RESERVED + 1) {
        end = node.end();
        size = node.size();
    } else {
        node_t temp = node_t(this, end, node.end(), delta - 2 * node_t::RESERVED, true);
        temp.write();
    }

    node_t temp = node_t(this, node.begin(), end, size, false);
    temp.write();

    allocations_count += 1;
    used_bytes += size;

    return temp.begin() + node_t::RESERVED;
}

bool buffer_t::free(uint16_t ptr) {
    node_t node(this, ptr - node_t::RESERVED);
    if (node.is_free()) {
        return false;
    }

    uint16_t begin = node.begin();
    uint16_t end = node.end();

    node_t begin_finder = node_t(this, 0, node.begin(), 0, false);
    begin_finder.read_end();
    if (begin_finder.is_valid() && begin_finder.is_free()) {
        begin = begin_finder.begin();
    }

    node_t end_finder(this, node.end());
    if (end_finder.is_free()) {
        end = end_finder.end();
    }

    allocations_count -= 1;
    used_bytes -= node.size();

    uint16_t size = end - begin - 2 * node_t::RESERVED;
    node_t temp = node_t(this, begin, end, size, true);
    temp.write();

    return true;
}

std::string buffer_t::info() {
    std::stringstream sstream;

    uint16_t result = 0;

    node_t node(this);
    while (node.is_valid()) {
        if (node.is_free()) {
            result = std::max(node.size(), result);
        }

        node.next();
    }

    sstream << allocations_count << " " << used_bytes << " " << result;

    return sstream.str();
}

std::string buffer_t::map() {
    std::stringstream sstream;

    node_t node(this);
    while (node.is_valid()) {
        sstream << "mm";
        sstream << std::string(node.size(), node.is_free() ? 'f' : 'u');
        sstream << "mm";

        node.next();
    }

    return sstream.str();
}

int main(int argc, const char *argv[])
{
    size_t buffer_size;
    std::cin >> buffer_size;
    if (buffer_size < 100 || buffer_size > 10000)
    {
        std::cerr << "Usage: <size> -- number in range of 100 and 10000" << std::endl;
        return 1;
    }

    buffer_t* buffer = new buffer_t(buffer_size);
    node_t::init_buffer(buffer);

    std::string command;
    while (std::cin >> command) {
        std::string result;

        if (command == "ALLOC") {
            uint16_t size;
            std::cin >> size;

            uint16_t p = buffer->alloc(size);

            std::stringstream sstream;
            if (p != 0) {
                sstream << "+ " << p;
            } else {
                sstream << "-";
            }

            result = sstream.str();
        } else if (command == "FREE") {
            uint16_t begin;
            std::cin >> begin;

            result = buffer->free(begin) ? "+" : "-";
        } else if (command == "INFO") {
            result = buffer->info();
        } else if (command == "MAP") {
            result = buffer->map();
        } else if (command == "QUIT")
        {
            break;
        }

        std::cout << result << std::endl;
    }

    delete buffer;

    return 0;
}