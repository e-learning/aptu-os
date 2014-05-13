#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include <list>
#include <cstdlib>
#include <vector>
#include <string>

#include "utils.h"

using std::list;
using std::string;
using std::vector;

template<class H, class T>
class BlockList {
public:
    BlockList(size_t bl_size)
        : head(bl_size)
        , block_size(bl_size)
    {}

    void Read(string const& root, size_t block_ptr);
    bool Write(string const& root, std::vector<size_t> block_ptrs);
    bool Write(string const& root);
    bool WriteExpanding(const std::string &root, std::vector<size_t> additional_blocks);
    void Add(T const& block) {
        elems_.push_back(block);
    }
    size_t Size() { return elems_.size() + 1; }

    typedef typename list<T>::iterator Iterator;
    Iterator ElemBegIterator() { return elems_.begin(); }
    Iterator EndIterator() { return elems_.end(); }
    T& Back() { return elems_.back(); }
    void PopBack() { elems_.pop_back(); }

    static vector<size_t> GetBlocksList(size_t block_size, const std::string &root, size_t first) {
        vector<size_t> blocks_list;
        blocks_list.push_back(first);
        H block(block_size, 'x');
        block.Read(Utils::ConstructPath(root, first));
        size_t block_ptr;
        while ((block_ptr = block.Next()) != 0) {
            blocks_list.push_back(block_ptr);
            block = H(block_size, 'x');
            block.Read(Utils::ConstructPath(root, block_ptr));
        }
        return blocks_list;
    }

    H head;
    size_t block_size;
private:
    list<T> elems_;
    std::vector<size_t> disk_blocks_;
};

#include "blocklist_impl.h"

#endif // BLOCKLIST_H
