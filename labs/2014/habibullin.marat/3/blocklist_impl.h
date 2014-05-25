#ifndef BLOCKLIST_IMPL_H
#define BLOCKLIST_IMPL_H

#include "utils.h"

template<class H, class T>
void BlockList<H, T>::Read(std::string const& root, size_t block_ptr) {
    disk_blocks_.push_back(block_ptr);
    string path = Utils::ConstructPath(root, block_ptr);
    head.Read(path);
    size_t next = head.Next();
    while(next) {
        disk_blocks_.push_back(next);
        T elem(block_size);
        path = Utils::ConstructPath(root, next);
        elem.Read(path);
        elems_.push_back(elem);
        next = elem.Next();
    }
}

template<class H, class T>
bool BlockList<H, T>::Write(std::string const& root, std::vector<size_t> block_ptrs) {
    if(block_ptrs.size() < elems_.size() + 1) {
        return false;
    }
    string path = Utils::ConstructPath(root, block_ptrs.at(0));
    size_t next = (block_ptrs.size() > 1) ? block_ptrs.at(1) : 0;
    head.SetNext(next);
    head.Write(path);
    size_t i = 1;
    typename list<T>::iterator iter;
    typename list<T>::iterator end;
    for (iter = elems_.begin(), end = elems_.end(); iter != end; ++iter) {
        path = Utils::ConstructPath(root, block_ptrs.at(i));
        next = (i + 1 != block_ptrs.size()) ? block_ptrs.at(i + 1) : 0;
        (*iter).SetNext(next);
        (*iter).Write(path);
        ++i;
    }
    return true;
}

template<class H, class T>
bool BlockList<H, T>::Write(const std::string &root) {
    return Write(root, disk_blocks_);
}

template<class H, class T>
bool BlockList<H, T>::WriteExpanding(const std::string &root, std::vector<size_t> additional_blocks) {
    disk_blocks_.insert(disk_blocks_.end(), additional_blocks.begin(), additional_blocks.end());
    return Write(root, disk_blocks_);
}

#endif // BLOCKLIST_IMPL_H
