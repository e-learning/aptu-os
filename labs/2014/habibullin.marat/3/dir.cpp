#include "dir.h"

#include <vector>

#include "utils.h"
#include "file.h"

using std::string;
using std::vector;

Dir::Dir(size_t block_size, string const& name) : blocks_(block_size) {
    blocks_.head.SetName(name);
    blocks_.head.SetSize(0);
    blocks_.head.SetLastModTime(Utils::CurrentDatetime());

    DirEntriesBlock deb(block_size);
    deb.SetEntriesNum(0);
    blocks_.Add(deb);
}

size_t Dir::TryFindEntry(std::string const& name) {
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator iter;
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator end = blocks_.EndIterator();
    for(iter = blocks_.ElemBegIterator(); iter != end; ++iter) {
        size_t res = (*iter).TryFindEntry(name);
        if(res != Utils::SizeTMax()) {
            return res;
        }
    }
    return Utils::SizeTMax();
}

bool Dir::AddEntryExpanding(const DirEntry &de) {
    if(blocks_.Back().TryAddEntry(de)) {
        return false;
    }
    DirEntriesBlock deb(blocks_.block_size);
    deb.TryAddEntry(de);
    blocks_.Add(deb);
    return true;
}

std::vector<std::string> Dir::EntriesList() {
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator iter;
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator end = blocks_.EndIterator();
    std::vector<std::string> entries;
    for(iter = blocks_.ElemBegIterator(); iter != end; ++iter) {
        size_t entries_num = (*iter).GetEntriesNum();
        for(size_t i = 0; i != entries_num; ++i) {
            entries.push_back((*iter).GetEntry(i).name);
        }
    }
    return entries;
}

size_t Dir::DeleteEntries(vector<size_t>& to_delete, size_t block_size, string const& root) {
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator iter;
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator end = blocks_.EndIterator();
    size_t freed_blocks = 0;
    for(iter = blocks_.ElemBegIterator(); iter != end; ++iter) {
        size_t entries_num = (*iter).GetEntriesNum();
        for(size_t i = 0; i != entries_num; ++i) {
            size_t child_ptr = (*iter).GetEntry(i).ptr;
            if(IsDir(root, child_ptr)) {
                Dir child(block_size, root, child_ptr);
                freed_blocks += child.DeleteEntries(to_delete, block_size, root);
            }
            else {
                File file(block_size, root, child_ptr);
                freed_blocks += file.SizeInBlocks();
            }
            to_delete.push_back(child_ptr);
        }
    }
    return freed_blocks;
}

size_t Dir::DeleteEntry(string const& name) {
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator iter;
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator end = blocks_.EndIterator();
    bool break_happened = false;
    size_t index;
    for(iter = blocks_.ElemBegIterator(); iter != end; ++iter) {
        size_t entries_num = (*iter).GetEntriesNum();
        for(index = 0; index != entries_num; ++index) {
            if(name == (*iter).GetEntry(index).name) {
                break_happened = true;
                break;
            }
        }
        if(break_happened) {
            break;
        }
    }
    size_t back_ent_num = blocks_.Back().GetEntriesNum();
    DirEntry last_entry = blocks_.Back().GetEntry(back_ent_num - 1);
    if(name != last_entry.name) {
        (*iter).SetEntry(index, last_entry);
    }
    blocks_.Back().SetEntriesNum(back_ent_num - 1);
    if(blocks_.Back().GetEntriesNum() == 0) {
        if(blocks_.Size() == 2) {
            return Utils::SizeTMax();
        }
        --iter;
        size_t ptr = (*iter).Next();
        (*iter).SetNext(0);
        blocks_.PopBack();
        return ptr;
    }
    return Utils::SizeTMax();
}

vector<SearchPath> Dir::CollectPaths(size_t block_size, string const& root) {
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator iter;
    typename BlockList<DirHeadBlock, DirEntriesBlock>::Iterator end = blocks_.EndIterator();
    vector<SearchPath> paths;
    for(iter = blocks_.ElemBegIterator(); iter != end; ++iter) {
        size_t entries_num = (*iter).GetEntriesNum();
        for(size_t i = 0; i != entries_num; ++i) {
            size_t child_ptr = (*iter).GetEntry(i).ptr;
            if(IsDir(root, child_ptr)) {
                Dir child(block_size, root, child_ptr);
                vector<SearchPath> collected = child.CollectPaths(block_size, root);
                paths.insert(paths.end(), collected.begin(), collected.end());
            }
            else {
                File file(block_size, root, child_ptr);
                SearchPath path_info = {file.Name(), child_ptr};
                paths.push_back(path_info);
            }
        }
    }
    if(paths.empty()) {
        SearchPath empdir_path = { Name(), 0 };
        paths.push_back(empdir_path);
    }
    else {
        for(size_t i = 0; i != paths.size(); ++i) {
            paths[i].path = Name() + '/' + paths[i].path;
        }
    }
    return paths;
}
