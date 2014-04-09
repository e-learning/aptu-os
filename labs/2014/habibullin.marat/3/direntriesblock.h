#ifndef DIRENTRIESBLOCK_H
#define DIRENTRIESBLOCK_H

#include "chainableblock.h"
#include "direntry.h"

class DirEntriesBlock : public ChainableBlock {
public:
    DirEntriesBlock(size_t s)
        : ChainableBlock(s, MAGIC)
        , entry_pos_(ENTRIESNUM_OFFS + SIZET_SIZE)
    {
        SetEntriesNum(0);
    }
    bool SetEntriesNum(size_t num) { return SetSizeT(num, ENTRIESNUM_OFFS); }
    size_t GetEntriesNum() { return GetSizeT(ENTRIESNUM_OFFS); }
    DirEntry GetEntry(size_t ind);
    bool SetEntry(size_t ind, DirEntry const& de);
    size_t TryFindEntry(std::string const& name);
    bool TryAddEntry(DirEntry const& de);

    size_t MaxCapacity() { return (size_ - EBODY_OFFS) / DirEntry::SIZE; }

private:
    const static char MAGIC = 'e';
    const static size_t ENTRIESNUM_OFFS = 5;
    const static size_t EBODY_OFFS = ENTRIESNUM_OFFS + SIZET_SIZE;

    size_t entry_pos_;
};

#endif // DIRENTRIESBLOCK_H
