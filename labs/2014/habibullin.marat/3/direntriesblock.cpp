#include "direntriesblock.h"

#include "utils.h"
#include "headblock.h"

DirEntry DirEntriesBlock::GetEntry(size_t ind) {
    size_t name_offset = EBODY_OFFS + ind * DirEntry::SIZE;
    size_t ptr_offset = name_offset + HeadBlock::NAME_MAX_SIZE;
    string raw_name = GetString(name_offset, HeadBlock::NAME_MAX_SIZE);
    size_t ptr = GetSizeT(ptr_offset);
    return DirEntry(Utils::FromRawString(raw_name), ptr);
}

bool DirEntriesBlock::SetEntry(size_t ind, DirEntry const& de) {
    size_t name_offset = EBODY_OFFS + ind * DirEntry::SIZE;
    size_t ptr_offset = name_offset + HeadBlock::NAME_MAX_SIZE;
    string normalized_name = Utils::ToRawString(de.name, HeadBlock::NAME_MAX_SIZE);
    return SetString(normalized_name, name_offset) && SetSizeT(de.ptr, ptr_offset);
}

size_t DirEntriesBlock::TryFindEntry(std::string const& name) {
    size_t entries_num = GetEntriesNum();
    for(size_t i = 0; i != entries_num; ++i) {
        DirEntry de = GetEntry(i);
        if(de.name == name) {
            return de.ptr;
        }
    }
    return Utils::SizeTMax();
}

bool DirEntriesBlock::TryAddEntry(const DirEntry &de) {
    size_t entries_num = GetEntriesNum();
    if(entries_num == MaxCapacity() || !SetEntry(entries_num, de)) {
        return false;
    }
    SetEntriesNum(entries_num + 1);
    return true;
}
